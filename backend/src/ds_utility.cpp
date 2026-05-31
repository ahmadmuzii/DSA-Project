#include "ds_utility.h"

std::string convertToComparable(const std::string& date) {
    if (date.length() != 10 || date[2] != ':' || date[5] != ':') {
        return date;
    }
    return date.substr(6, 4) + date.substr(3, 2) + date.substr(0, 2);
}

int timeToMinutes(const std::string& time) {
    if (time.length() != 5 || time[2] != ':') return 0;
    return std::stoi(time.substr(0, 2)) * 60 + std::stoi(time.substr(3, 2));
}

std::string escapeCSV(const std::string& field) {
    bool needsQuoting = false;
    for (char c : field) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') {
            needsQuoting = true;
            break;
        }
    }
    if (!needsQuoting) return field;

    std::string result = "\"";
    for (char c : field) {
        if (c == '"') result += "\"\"";
        else result += c;
    }
    result += "\"";
    return result;
}

std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                current += '"';
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}
