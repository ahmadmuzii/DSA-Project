#include "file_manager.h"
#include "calendar.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <unordered_set>

using namespace std;

void saveRec(BSTNode* node, ofstream& file) {
    if (!node) return;
    saveRec(node->left, file);
    
    ListNode* curr = node->events.head;
    while(curr) {
        Event e = curr->data;
        file << e.id << "," << e.title << "," << e.date << "," 
             << e.startTime << "," << e.durationMins << "," << e.priority << "\n";
        curr = curr->next;
    }
    
    saveRec(node->right, file);
}

void saveRec(BSTNode* node, ofstream& file, unordered_set<string>& savedIDs) {
    if (!node) return;
    saveRec(node->left, file, savedIDs);
    
    ListNode* curr = node->events.head;
    while(curr) {
        // Check if event ID already saved (to avoid duplicates)
        if (savedIDs.find(curr->data.id) == savedIDs.end()) {
            Event e = curr->data;
            file << e.id << "," << e.title << "," << e.date << "," 
                 << e.startTime << "," << e.durationMins << "," << e.priority << "\n";
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
    
    unordered_set<string> savedIDs; // Track saved event IDs
    saveRec(bst->root, outFile, savedIDs);
    outFile.close();
    
    cout << "\n==============================================================\n";
    cout << "                    DATA SAVED SUCCESSFULLY!             \n";
    cout << "==============================================================\n";
    cout << "Saved " << savedIDs.size() << " unique events to: " << filename << "\n";
}

void loadEventsFromFile(string filename, CalendarSystem& sys) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "\n[INFO] No saved data found. Starting with empty calendar.\n";
        return;
    }
    
    // Clear existing events first (or the CalendarSystem should have a clear() method)
    // We'll need to add a clear method to CalendarSystem
    
    string line;
    int loaded = 0;
    int duplicates = 0;
    unordered_set<string> loadedIDs; // Track loaded IDs to avoid duplicates
    
    while (getline(inFile, line)) {
        if(line.empty()) continue;
        
        Event e;
        string temp = "";
        int commaCount = 0;
        
        for(char c : line) {
            if(c == ',') {
                if(commaCount == 0) e.id = temp;
                else if(commaCount == 1) e.title = temp;
                else if(commaCount == 2) e.date = temp;
                else if(commaCount == 3) e.startTime = temp;
                else if(commaCount == 4) e.durationMins = stoi(temp);
                
                temp = "";
                commaCount++;
            } else {
                temp += c;
            }
        }
        e.priority = stoi(temp);
        
        // Check for duplicate ID before adding
        if (loadedIDs.find(e.id) != loadedIDs.end()) {
            duplicates++;
            continue;
        }
        
        sys.addEventDirect(e);
        loadedIDs.insert(e.id);
        loaded++;
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