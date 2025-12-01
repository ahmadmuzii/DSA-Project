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

public:
    CalendarSystem() : eventCounter(0) {}

    // Core Functions
    bool addEvent(string title, string date, string start, int duration, int priority, string desc);
    void viewEventsByDate(string date);
    void searchEvents(string query); // Searches both BST (dates) and Map (IDs) logic
    void checkConflicts(string date); 
    void viewUpcoming();
    void displayStructures();
    
    // Helper for saving
    BST* getBST() { return &eventTree; }
    
    // For loading
    void addEventDirect(Event e); 
};

#endif