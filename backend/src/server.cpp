#include "httplib.h"
#include "calendar.h"
#include "file_manager.h"
#include "config.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <csignal>
#include <algorithm>
#include <fstream>
#include <iterator>

using namespace std;

static volatile sig_atomic_t shutdownRequested = 0;
static httplib::Server* globalServer = nullptr;
static CalendarSystem* globalCalendar = nullptr;

static void signalHandler(int sig) {
    shutdownRequested = 1;
    if (globalServer) {
        globalServer->stop();
    }
}

static string escapeJSON(const string& s) {
    string out;
    out.reserve(s.length());
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c;
        }
    }
    return out;
}

// Simple JSON string value extractor for flat JSON objects.
// Finds "key":"value" and returns the unescaped value.
static string getJSONString(const string& body, const string& key) {
    string target = "\"" + key + "\":\"";
    size_t start = body.find(target);
    if (start == string::npos) return "";

    start += target.length();
    string result;
    for (size_t i = start; i < body.length(); i++) {
        if (body[i] == '\\' && i + 1 < body.length()) {
            result += body[i + 1];
            i++;
        } else if (body[i] == '"') {
            break;
        } else {
            result += body[i];
        }
    }
    return result;
}

// Extracts a numeric value for a key from JSON: "key":123
static string getJSONNumber(const string& body, const string& key, const string& defaultVal) {
    string target = "\"" + key + "\":";
    size_t start = body.find(target);
    if (start == string::npos) return defaultVal;

    start += target.length();
    string result;
    for (size_t i = start; i < body.length(); i++) {
        if (body[i] >= '0' && body[i] <= '9') {
            result += body[i];
        } else if (body[i] == '-') {
            result += body[i];
        } else {
            break;
        }
    }
    return result.empty() ? defaultVal : result;
}

// Returns true if the request body is JSON
static bool isJSONRequest(const httplib::Request& req) {
    string ct = req.get_header_value("Content-Type");
    return ct.find("application/json") != string::npos;
}

static string paramOrJSON(const httplib::Request& req, const string& key, const string& defaultVal) {
    if (isJSONRequest(req)) {
        string val = getJSONString(req.body, key);
        return val.empty() ? getJSONNumber(req.body, key, defaultVal) : val;
    }
    return req.get_param_value_or(key, defaultVal);
}

static string eventToJSON(const Event& e) {
    stringstream ss;
    ss << "{"
        << "\"id\":\"" << escapeJSON(e.id) << "\","
        << "\"title\":\"" << escapeJSON(e.title) << "\","
        << "\"date\":\"" << escapeJSON(e.date) << "\","
        << "\"startTime\":\"" << escapeJSON(e.startTime) << "\","
        << "\"endTime\":\"" << escapeJSON(e.getEndTime()) << "\","
        << "\"durationMins\":" << e.durationMins << ","
        << "\"priority\":" << e.priority << ","
        << "\"description\":\"" << escapeJSON(e.description) << "\""
        << "}";
    return ss.str();
}

static string eventsToJSON(const vector<Event>& events) {
    string s = "[";
    for (size_t i = 0; i < events.size(); i++) {
        if (i > 0) s += ",";
        s += eventToJSON(events[i]);
    }
    s += "]";
    return s;
}

static string paginatedEventsJSON(const vector<Event>& all, int page, int perPage) {
    int total = (int)all.size();
    int start = (page - 1) * perPage;
    int end = min(start + perPage, total);
    int totalPages = max(1, (total + perPage - 1) / perPage);

    string json = R"({"events":)";
    json += "[";
    for (int i = start; i < end; i++) {
        if (i > start) json += ",";
        json += eventToJSON(all[i]);
    }
    json += R"(],"total":)" + to_string(total)
         + R"(,"page":)" + to_string(page)
         + R"(,"perPage":)" + to_string(perPage)
         + R"(,"totalPages":)" + to_string(totalPages)
         + "}";
    return json;
}

int main() {
    CalendarSystem calendar;
    globalCalendar = &calendar;

    // Auto-load on startup
    ifstream checkFile(Config::DATA_FILE);
    if (checkFile.good()) {
        checkFile.close();
        loadEventsFromFile(Config::DATA_FILE, calendar);
    }

    httplib::Server svr;
    globalServer = &svr;

    // Register signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // CORS middleware
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"},
        {"Access-Control-Expose-Headers", "Content-Type"}
    });

    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 204;
    });

    // POST /api/events - Add event
    svr.Post("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
        auto title = paramOrJSON(req, "title", "");
        auto date = paramOrJSON(req, "date", "");
        auto start = paramOrJSON(req, "startTime", "");
        auto durStr = paramOrJSON(req, "duration", "0");
        auto prioStr = paramOrJSON(req, "priority", "3");
        auto desc = paramOrJSON(req, "description", "");

        if (title.empty()) {
            res.status = 400;
            res.set_content(R"({"success":false,"error":"Title is required"})", "application/json");
            return;
        }
        if (date.length() != 10 || date[2] != ':' || date[5] != ':') {
            res.status = 400;
            res.set_content(R"({"success":false,"error":"Date must be DD:MM:YYYY"})", "application/json");
            return;
        }
        if (start.length() != 5 || start[2] != ':') {
            res.status = 400;
            res.set_content(R"({"success":false,"error":"Time must be HH:MM"})", "application/json");
            return;
        }

        int dur = 0, prio = 3;
        try { dur = stoi(durStr); } catch (...) { dur = 0; }
        try { prio = stoi(prioStr); } catch (...) { prio = 3; }
        if (dur < 1) dur = 1;
        if (prio < 1) prio = 1;
        if (prio > 5) prio = 5;

        bool success = calendar.addEvent(title, date, start, dur, prio, desc);
        if (success) {
            res.status = 201;
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.status = 409;
            res.set_content(R"({"success":false,"error":"Time conflict detected"})", "application/json");
        }
    });

    // GET /api/events - Get all upcoming (paginated)
    svr.Get("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
        int page = stoi(req.get_param_value_or("page", "1"));
        int perPage = stoi(req.get_param_value_or("per_page", "12"));
        if (page < 1) page = 1;
        if (perPage < 1) perPage = 12;
        if (perPage > 100) perPage = 100;

        auto events = calendar.viewUpcoming(500);
        res.set_content(paginatedEventsJSON(events, page, perPage), "application/json");
    });

    // GET /api/events/date/:date - Get events by date
    svr.Get(R"(/api/events/date/(\d{2}:\d{2}:\d{4}))", [&](const httplib::Request& req, httplib::Response& res) {
        string date = req.matches[1];
        auto events = calendar.getEventsByDate(date);
        res.set_content(eventsToJSON(events), "application/json");
    });

    // GET /api/events/search - Search events (query param: q)
    svr.Get("/api/events/search", [&](const httplib::Request& req, httplib::Response& res) {
        string q = req.get_param_value_or("q", "");
        auto events = calendar.searchEvents(q);
        res.set_content(eventsToJSON(events), "application/json");
    });

    // GET /api/events/conflicts/:date - Check conflicts
    svr.Get(R"(/api/events/conflicts/(\d{2}:\d{2}:\d{4}))", [&](const httplib::Request& req, httplib::Response& res) {
        string date = req.matches[1];
        auto result = calendar.checkConflicts(date);
        string json = "{\"conflicts\":[";
        for (size_t i = 0; i < result.conflicts.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"eventA\":" + eventToJSON(result.conflicts[i].a) +
                    ",\"eventB\":" + eventToJSON(result.conflicts[i].b) + "}";
        }
        json += "],\"conflictCount\":" + to_string(result.conflicts.size()) + "}";
        res.set_content(json, "application/json");
    });

    // GET /api/events/:id - Get single event by ID
    svr.Get(R"(/api/events/(EVT_\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        string id = req.matches[1];
        Event* e = calendar.getEventById(id);
        if (e) {
            res.set_content(eventToJSON(*e), "application/json");
        } else {
            res.status = 404;
            res.set_content(R"({"error":"Event not found"})", "application/json");
        }
    });

    // PUT /api/events/:id - Update event
    svr.Put(R"(/api/events/(EVT_\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        string id = req.matches[1];
        auto title = paramOrJSON(req, "title", "");
        auto date = paramOrJSON(req, "date", "");
        auto start = paramOrJSON(req, "startTime", "");
        auto durStr = paramOrJSON(req, "duration", "0");
        auto prioStr = paramOrJSON(req, "priority", "3");
        auto desc = paramOrJSON(req, "description", "");

        Event* ev = calendar.getEventById(id);
        if (!ev) {
            res.status = 404;
            res.set_content(R"({"success":false,"error":"Event not found"})", "application/json");
            return;
        }

        if (title.empty()) title = ev->title;
        if (date.empty()) date = ev->date;
        if (start.empty()) start = ev->startTime;
        int dur, prio;
        try { dur = durStr == "0" ? ev->durationMins : stoi(durStr); } catch (...) { dur = ev->durationMins; }
        try { prio = prioStr == "3" ? ev->priority : stoi(prioStr); } catch (...) { prio = ev->priority; }
        if (desc.empty()) desc = ev->description;

        bool success = calendar.updateEvent(id, title, date, start, dur, prio, desc);
        if (success) {
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.status = 409;
            res.set_content(R"({"success":false,"error":"Update failed"})", "application/json");
        }
    });

    // DELETE /api/events/:id - Delete event
    svr.Delete(R"(/api/events/(EVT_\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        string id = req.matches[1];
        bool success = calendar.deleteEvent(id);
        if (success) {
            res.status = 200;
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.status = 404;
            res.set_content(R"({"success":false,"error":"Event not found"})", "application/json");
        }
    });

    // GET /api/structures - Data structure info
    svr.Get("/api/structures", [&](const httplib::Request&, httplib::Response& res) {
        string info = calendar.displayStructures();
        string json = R"({"info":")" + escapeJSON(info) + "\"}";
        res.set_content(json, "application/json");
    });

    // POST /api/save - Save to file
    svr.Post("/api/save", [&](const httplib::Request&, httplib::Response& res) {
        bool success = saveEventsToFile(Config::DATA_FILE, calendar);
        if (success) {
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.set_content(R"({"success":false,"error":"Save failed"})", "application/json");
        }
    });

    // POST /api/load - Load from file
    svr.Post("/api/load", [&](const httplib::Request&, httplib::Response& res) {
        calendar.clearAllEvents();
        int count = loadEventsFromFile(Config::DATA_FILE, calendar);
        string json = R"({"success":true,"count":)" + to_string(count) + "}";
        res.set_content(json, "application/json");
    });

    Logger::info("Calendar API Server starting...");
    Logger::info("Listening on http://localhost:8080");
    Logger::info("Endpoints: GET/POST /api/events, GET /api/events/date/:date, GET /api/events/search");
    Logger::info("           GET/PUT/DELETE /api/events/:id, GET /api/events/conflicts/:date");
    Logger::info("           GET /api/structures, GET /api/export, POST /api/save, POST /api/load");

    // GET /api/export - Export formatted TXT
    svr.Get("/api/export", [&](const httplib::Request&, httplib::Response& res) {
        if (!calendar.exportFormattedTXT(Config::EXPORT_FILE)) {
            res.status = 500;
            res.set_content(R"({"success":false,"error":"Export failed"})", "application/json");
            return;
        }
        ifstream file(Config::EXPORT_FILE);
        if (!file) {
            res.status = 500;
            res.set_content(R"({"success":false,"error":"Failed to read export file"})", "application/json");
            return;
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        res.set_content(content, "text/plain");
    });

    svr.listen("0.0.0.0", 8080);

    // Graceful shutdown: save data before exiting
    Logger::info("Shutting down, saving data...");
    saveEventsToFile(Config::DATA_FILE, calendar);
    Logger::info("Server stopped.");
    return 0;
}
