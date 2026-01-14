#include "file_manager.h"
#include "calendar.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

void saveRec(BSTNode* node, ofstream& file) {
    if (!node) {
        return;
    }
    saveRec(node->left, file);
    
    ListNode* curr = node->events.head;
    while (curr) {
        Event e = curr->data;
        file << e.id << "," << e.title << "," << e.date << ","  
             << e.startTime << "," << e.durationMins << "," << e.priority 
             << "," << e.description << "\n";
        curr = curr->next;
    }
    
    saveRec(node->right, file);
}

void saveRec(BSTNode* node, ofstream& file, unordered_set<string>& savedIDs) {
    if (!node) {
        return;
    }
    saveRec(node->left, file, savedIDs);
    
    ListNode* curr = node->events.head;
    while (curr) {
        if (savedIDs.find(curr->data.id) == savedIDs.end()) {
            Event e = curr->data;
            file << e.id << "," << e.title << "," << e.date << ","  
                 << e.startTime << "," << e.durationMins << "," << e.priority 
                 << "," << e.description << "\n";
            savedIDs.insert(e.id);
        }
        curr = curr->next;
    }
    
    saveRec(node->right, file, savedIDs);
}

void saveEventsToFile(string filename, BST* bst) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "\n[ERROR] Error opening file for save!\n";
        return;
    }
    
    unordered_set<string> savedIDs;
    saveRec(bst->root, outFile, savedIDs);
    outFile.close();
    
    cout << "\n-----------------------------------------------------------\n";
    cout << "                    DATA SAVED SUCCESSFULLY!             \n";
    cout << "-----------------------------------------------------------\n";
    cout << "Saved " << savedIDs.size() << " unique events to: " << filename << "\n";
}

// Helper function to extract counter from new ID format
int extractCounterFromID(const string& id) {
    
    if (id.find("EVT_") != 0) {
        return 0; // Not our format
    }
    
    size_t lastUnderscore = id.find_last_of('_');
    if (lastUnderscore == string::npos || lastUnderscore == 3) {
        string numStr = id.substr(4); 
        try {
            return stoi(numStr) - 12340; // Convert to counter
        } catch (...) {
            return 0;
        }
    }
    
    // Get the part after last underscore
    string counterStr = id.substr(lastUnderscore + 1);
    try {
        return stoi(counterStr) - 12340; // Convert to counter
    } catch (...) {
        size_t secondLast = id.find_last_of('_', lastUnderscore - 1);
        if (secondLast != string::npos) {
            counterStr = id.substr(secondLast + 1, lastUnderscore - secondLast - 1);
            try {
                return stoi(counterStr) - 12340;
            } catch (...) {
                return 0;
            }
        }
    }
    
    return 0;
}

void loadEventsFromFile(string filename, CalendarSystem& sys) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "\n[INFO] No saved data found. Starting with empty calendar.\n";
        return;
    }
    
    string line;
    int loaded = 0;
    int duplicates = 0;
    unordered_set<string> loadedIDs;
    
    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }
        
        Event e;
        stringstream ss(line);
        string token;
        vector<string> tokens;

        // Split line by commas
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 6) {
            e.id = tokens[0];
            e.title = tokens[1];
            e.date = tokens[2];
            e.startTime = tokens[3];
            e.durationMins = stoi(tokens[4]);
            e.priority = stoi(tokens[5]);
            
            if (tokens.size() >= 7) {
                e.description = tokens[6];
            } else {
                e.description = "";  // Default empty description
            }
            
            // Check for duplicates
            if (loadedIDs.find(e.id) != loadedIDs.end()) {
                duplicates++;
                continue;
            }
            
            sys.addEventDirect(e);
            loadedIDs.insert(e.id);
            loaded++;
        }
    }
    
    cout << "\n==============================================================\n";
    cout << "                    DATA LOADED SUCCESSFULLY!            \n";
    cout << "==============================================================\n";
    cout << "Loaded " << loaded << " unique events from " << filename << "\n";
    if (duplicates > 0) {
        cout << "Skipped " << duplicates << " duplicate entries\n";
    }
    
    inFile.close();
}