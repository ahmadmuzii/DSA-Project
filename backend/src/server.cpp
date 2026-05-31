#include "httplib.h"
#include "calendar.h"
#include "file_manager.h"
#include <iostream>
#include <sstream>

using namespace std;

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

int main() {
    CalendarSystem calendar;

    // Auto-load on startup
    ifstream checkFile("calendar_data.txt");
    if (checkFile.good()) {
        checkFile.close();
        loadEventsFromFile("calendar_data.txt", calendar);
    }

    httplib::Server svr;

    // CORS middleware
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 204;
    });

    // POST /api/events - Add event
    svr.Post("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
        auto title = req.get_param_value_or("title", "");
        auto date = req.get_param_value_or("date", "");
        auto start = req.get_param_value_or("startTime", "");
        auto durStr = req.get_param_value_or("duration", "0");
        auto prioStr = req.get_param_value_or("priority", "3");
        auto desc = req.get_param_value_or("description", "");

        int dur = stoi(durStr);
        int prio = stoi(prioStr);

        bool success = calendar.addEvent(title, date, start, dur, prio, desc);
        if (success) {
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.set_content(R"({"success":false,"error":"Time conflict detected"})", "application/json");
        }
    });

    // GET /api/events - Get all upcoming
    svr.Get("/api/events", [&](const httplib::Request&, httplib::Response& res) {
        auto events = calendar.viewUpcoming(50);
        res.set_content(eventsToJSON(events), "application/json");
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
            res.set_content(R"({"error":"Event not found"})", "application/json");
        }
    });

    // PUT /api/events/:id - Update event
    svr.Put(R"(/api/events/(EVT_\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        string id = req.matches[1];
        auto title = req.get_param_value_or("title", "");
        auto date = req.get_param_value_or("date", "");
        auto start = req.get_param_value_or("startTime", "");
        auto durStr = req.get_param_value_or("duration", "0");
        auto prioStr = req.get_param_value_or("priority", "3");
        auto desc = req.get_param_value_or("description", "");

        Event* ev = calendar.getEventById(id);
        if (!ev) {
            res.set_content(R"({"success":false,"error":"Event not found"})", "application/json");
            return;
        }

        if (title.empty()) title = ev->title;
        if (date.empty()) date = ev->date;
        if (start.empty()) start = ev->startTime;
        int dur = durStr == "0" ? ev->durationMins : stoi(durStr);
        int prio = prioStr == "3" ? ev->priority : stoi(prioStr);
        if (desc.empty()) desc = ev->description;

        bool success = calendar.updateEvent(id, title, date, start, dur, prio, desc);
        if (success) {
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.set_content(R"({"success":false,"error":"Update failed"})", "application/json");
        }
    });

    // DELETE /api/events/:id - Delete event
    svr.Delete(R"(/api/events/(EVT_\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        string id = req.matches[1];
        bool success = calendar.deleteEvent(id);
        if (success) {
            res.set_content(R"({"success":true})", "application/json");
        } else {
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
        bool success = saveEventsToFile("calendar_data.txt", calendar);
        if (success) {
            res.set_content(R"({"success":true})", "application/json");
        } else {
            res.set_content(R"({"success":false,"error":"Save failed"})", "application/json");
        }
    });

    // POST /api/load - Load from file
    svr.Post("/api/load", [&](const httplib::Request&, httplib::Response& res) {
        calendar.clearAllEvents();
        int count = loadEventsFromFile("calendar_data.txt", calendar);
        string json = R"({"success":true,"count":)" + to_string(count) + "}";
        res.set_content(json, "application/json");
    });

    cout << "\n============================================\n";
    cout << "  Calendar API Server starting...          \n";
    cout << "  Listening on http://localhost:8080       \n";
    cout << "  API endpoints:                           \n";
    cout << "    GET  /api/events                       \n";
    cout << "    GET  /api/events/date/:date            \n";
    cout << "    GET  /api/events/search?q=             \n";
    cout << "    GET  /api/events/conflicts/:date       \n";
    cout << "    GET  /api/events/:id                   \n";
    cout << "    POST /api/events                       \n";
    cout << "    PUT  /api/events/:id                   \n";
    cout << "    DELETE /api/events/:id                 \n";
    cout << "    GET  /api/structures                   \n";
    cout << "    POST /api/save                         \n";
    cout << "    POST /api/load                         \n";
    cout << "============================================\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
