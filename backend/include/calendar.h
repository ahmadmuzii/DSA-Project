#ifndef CALENDAR_H
#define CALENDAR_H

#include "ds_lib.h"
#include <string>
#include <vector>
#include <utility>

struct ConflictResult {
    struct ConflictPair {
        Event a;
        Event b;
    };
    std::vector<ConflictPair> conflicts;
};

class CalendarSystem {
private:
    BST eventTree;
    HashTable eventMap;
    MinHeap eventHeap;
    int eventCounter;

    void syncCounterFromID(const std::string& id);

public:
    CalendarSystem() : eventCounter(0) {}

    bool addEvent(const std::string& title, const std::string& date,
                  const std::string& start, int duration, int priority,
                  const std::string& desc);
    void addEventDirect(Event e);

    std::vector<Event> getEventsByDate(const std::string& date);
    std::vector<Event> searchEvents(const std::string& query);
    ConflictResult checkConflicts(const std::string& date);
    std::vector<Event> viewUpcoming(int count = 10);
    std::string displayStructures() const;

    bool updateEvent(const std::string& id, const std::string& newTitle,
                     const std::string& newDate, const std::string& newTime,
                     int newDuration, int newPriority, const std::string& newDesc);
    bool deleteEvent(const std::string& id);

    Event* getEventById(const std::string& id) {
        return eventMap.search(id);
    }

    bool exportFormattedTXT(const std::string& filename) const;
    void clearAllEvents();
    void forEachEvent(std::function<void(const Event&)> callback) const;

    BST* getBST() { return &eventTree; }
};

#endif
