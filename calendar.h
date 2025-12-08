#ifndef CALENDAR_H
#define CALENDAR_H

#include "ds_lib.h"
#include <string>

class CalendarSystem {
private:
    BST eventTree;      // Date-based retrieval
    HashTable eventMap; // ID-based retrieval
    MinHeap eventHeap;  // Priority/Upcoming retrieval
    int eventCounter;   // For generating IDs

    // Helper methods for deletion
    bool deleteFromBST(string date, string id);
    bool deleteFromHeap(string id);
    bool deleteFromHash(string id);
    
    // Helper for BST traversal
    BSTNode* findDateNodeByEventId(string id);

public:
    CalendarSystem() : eventCounter(0) {}

    // Core Functions
    bool addEvent(string title, string date, string start, int duration, int priority, string desc);
    void viewEventsByDate(string date);
    void searchEvents(string query); // Searches both BST (dates) and Map (IDs) logic
    void checkConflicts(string date); 
    void viewUpcoming();
    void displayStructures();
    
    // New Functions
    bool updateEvent(string id);
    bool deleteEvent(string id);
    void exportFormattedTXT(string filename);
    
    // Helper for saving
    BST* getBST() { return &eventTree; }
    
    // For loading
    void addEventDirect(Event e); 
    
    // Getter for event map (for updates)
    Event* getEventById(string id) { return eventMap.search(id); }

    //For clear
    void clearAllEvents();
};

#endif