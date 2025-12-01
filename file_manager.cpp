#include "file_manager.h"
#include "calendar.h"
#include <fstream>
#include <iostream>

using namespace std;

// Recursive helper to save all events in BST
void saveRec(BSTNode* node, ofstream& file) {
    if (!node) return;
    saveRec(node->left, file);
    
    ListNode* curr = node->events.head;
    while(curr) {
        Event e = curr->data;
        // Format: ID,Title,Date,Start,Duration,Priority
        file << e.id << "," << e.title << "," << e.date << "," 
             << e.startTime << "," << e.durationMins << "," << e.priority << "\n";
        curr = curr->next;
    }
    
    saveRec(node->right, file);
}

void saveEventsToFile(string filename, BST* bst) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Error opening file for save.\n";
        return;
    }
    
    saveRec(bst->root, outFile);
    outFile.close();
    cout << "\n✅ Data saved successfully to " << filename << "\n";
}

void loadEventsFromFile(string filename, CalendarSystem& sys) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "No saved data found (or error opening file).\n";
        return;
    }
    
    string line;
    int loaded = 0;
    while (getline(inFile, line)) {
        if(line.empty()) continue;
        
        // Manual parsing of CSV line
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
        e.priority = stoi(temp); // Last item
        
        sys.addEventDirect(e);
        loaded++;
    }
    cout << "\n✅ Loaded " << loaded << " events from file.\n";
    inFile.close();
}