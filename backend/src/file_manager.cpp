#include "file_manager.h"
#include "calendar.h"
#include "ds_utility.h"
#include <fstream>
#include <iostream>
#include <unordered_set>

static int extractCounterFromID(const std::string& id) {
    if (id.find("EVT_") != 0) return 0;

    std::string numStr = id.substr(4);
    try {
        return std::stoi(numStr) - 12340;
    } catch (...) {
        return 0;
    }
}

bool saveEventsToFile(const std::string& filename, CalendarSystem& calendar) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cout << "\n[ERROR] Error opening file for save!\n";
        return false;
    }

    std::unordered_set<std::string> savedIDs;
    int count = 0;
    calendar.forEachEvent([&](const Event& e) {
        if (savedIDs.find(e.id) == savedIDs.end()) {
            outFile << e.id << ","
                    << escapeCSV(e.title) << ","
                    << e.date << ","
                    << e.startTime << ","
                    << e.durationMins << ","
                    << e.priority << ","
                    << escapeCSV(e.description) << "\n";
            savedIDs.insert(e.id);
            count++;
        }
    });

    outFile.close();

    if (count > 0) {
        std::cout << "\n-----------------------------------------------------------\n";
        std::cout << "                    DATA SAVED SUCCESSFULLY!             \n";
        std::cout << "-----------------------------------------------------------\n";
        std::cout << "Saved " << count << " unique events to: " << filename << "\n";
    } else {
        std::cout << "\n[INFO] No events to save.\n";
    }

    return true;
}

int loadEventsFromFile(const std::string& filename, CalendarSystem& calendar) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cout << "\n[INFO] No saved data found. Starting with empty calendar.\n";
        return 0;
    }

    std::string line;
    int loaded = 0;
    int duplicates = 0;
    std::unordered_set<std::string> loadedIDs;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        auto tokens = parseCSVLine(line);
        if (tokens.size() < 6) continue;

        Event e;
        e.id = tokens[0];
        e.title = tokens[1];
        e.date = tokens[2];
        e.startTime = tokens[3];

        try {
            e.durationMins = std::stoi(tokens[4]);
            e.priority = std::stoi(tokens[5]);
        } catch (...) {
            continue;
        }

        e.description = (tokens.size() >= 7) ? tokens[6] : "";

        if (loadedIDs.find(e.id) != loadedIDs.end()) {
            duplicates++;
            continue;
        }

        calendar.addEventDirect(e);
        loadedIDs.insert(e.id);
        loaded++;
    }

    inFile.close();

    std::cout << "\n==============================================================\n";
    std::cout << "                    DATA LOADED SUCCESSFULLY!            \n";
    std::cout << "==============================================================\n";
    std::cout << "Loaded " << loaded << " unique events from " << filename << "\n";
    if (duplicates > 0) {
        std::cout << "Skipped " << duplicates << " duplicate entries\n";
    }

    return loaded;
}
