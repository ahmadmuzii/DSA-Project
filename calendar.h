#ifndef CALENDAR_H
#define CALENDAR_H

#include "ds_lib.h"
#include <string>

class CalendarSystem {
private:
    BST eventTree;
    HashTable eventMap;
    MinHeap eventHeap;
    int eventCounter;

    bool deleteFromBST(string date, string id);
    bool deleteFromHeap(string id);
    bool deleteFromHash(string id);
    
    BSTNode* findDateNodeByEventId(string id);

public:
    CalendarSystem() : eventCounter(0) {}

    bool addEvent(string title, string date, string start, int duration, int priority, string desc);
    void viewEventsByDate(string date);
    void searchEvents(string query);
    void checkConflicts(string date); 
    void viewUpcoming();
    void displayStructures();
    
    bool updateEvent(string id);
    bool deleteEvent(string id);
    void exportFormattedTXT(string filename);
    
    BST* getBST() { 
        return &eventTree; 
    }
    
    void addEventDirect(Event e); 
    
    Event* getEventById(string id) { 
        return eventMap.search(id); 
    }

    void clearAllEvents();
};

#endif