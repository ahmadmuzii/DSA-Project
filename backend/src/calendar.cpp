#include "calendar.h"
#include "ds_utility.h"
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <functional>

static int& getEventCounter() {
    static int counter = -1;
    static int unsavedChanges = 0;

    if (counter == -1) {
        std::ifstream file("event_counter.txt");
        if (file) {
            file >> counter;
            if (counter < 0) counter = 0;
        } else {
            counter = 0;
        }

        std::atexit([]() {
            std::ofstream file("event_counter.txt");
            if (file) file << counter;
        });
    }

    return counter;
}

static std::string generateID() {
    int& counter = getEventCounter();
    counter++;

    static int& unsaved = *new int(0);
    unsaved++;
    if (unsaved >= 10) {
        std::ofstream file("event_counter.txt");
        if (file) file << counter;
        unsaved = 0;
    }

    return "EVT_" + std::to_string(12340 + counter);
}

static BSTNode* findDateNodeRec(BSTNode* node, const std::string& id) {
    if (!node) return nullptr;

    ListNode* curr = node->events.getHead();
    while (curr) {
        if (curr->data.id == id) return node;
        curr = curr->next.get();
    }

    BSTNode* leftResult = findDateNodeRec(node->left.get(), id);
    if (leftResult) return leftResult;

    return findDateNodeRec(node->right.get(), id);
}

void CalendarSystem::syncCounterFromID(const std::string& id) {
    if (id.find("EVT_") != std::string::npos) {
        std::string numStr = id.substr(4);
        try {
            int num = std::stoi(numStr);
            if (num >= 12341) {
                int c = num - 12340;
                if (c > eventCounter) eventCounter = c;
            }
        } catch (...) {}
    }
}

bool CalendarSystem::addEvent(const std::string& title, const std::string& date,
                               const std::string& start, int duration, int priority,
                               const std::string& desc) {
    Event newEvent;
    newEvent.title = title;
    newEvent.date = date;
    newEvent.startTime = start;
    newEvent.durationMins = duration;
    newEvent.priority = priority;
    newEvent.description = desc;

    std::string endTime = newEvent.getEndTime();

    BSTNode* dateNode = eventTree.search(date);
    if (dateNode) {
        Event* conflict = dateNode->events.checkConflict(start, endTime);
        if (conflict) return false;
    }

    newEvent.id = generateID();

    eventTree.insert(date, newEvent);
    eventMap.insert(newEvent);
    eventHeap.insert(newEvent);

    return true;
}

void CalendarSystem::addEventDirect(Event e) {
    if (eventMap.search(e.id)) return;

    eventTree.insert(e.date, e);
    eventMap.insert(e);
    eventHeap.insert(e);
    syncCounterFromID(e.id);
}

std::vector<Event> CalendarSystem::getEventsByDate(const std::string& date) {
    std::vector<Event> result;
    BSTNode* node = eventTree.search(date);
    if (!node) return result;

    ListNode* curr = node->events.getHead();
    while (curr) {
        result.push_back(curr->data);
        curr = curr->next.get();
    }
    return result;
}

std::vector<Event> CalendarSystem::searchEvents(const std::string& query) {
    std::vector<Event> results;
    eventTree.forEach([&](const Event& e) {
        if (e.title.find(query) != std::string::npos ||
            e.id.find(query) != std::string::npos) {
            results.push_back(e);
        }
    });
    return results;
}

ConflictResult CalendarSystem::checkConflicts(const std::string& date) {
    ConflictResult result;
    BSTNode* node = eventTree.search(date);
    if (!node) return result;

    std::vector<Event> events;
    ListNode* curr = node->events.getHead();
    while (curr) {
        events.push_back(curr->data);
        curr = curr->next.get();
    }

    for (size_t i = 0; i < events.size(); i++) {
        for (size_t j = i + 1; j < events.size(); j++) {
            int aStart = timeToMinutes(events[i].startTime);
            int aEnd = timeToMinutes(events[i].getEndTime());
            int bStart = timeToMinutes(events[j].startTime);
            int bEnd = timeToMinutes(events[j].getEndTime());

            if (aStart < bEnd && aEnd > bStart) {
                result.conflicts.push_back({events[i], events[j]});
            }
        }
    }

    return result;
}

std::vector<Event> CalendarSystem::viewUpcoming(int count) {
    std::vector<Event> result;
    if (eventHeap.isEmpty()) return result;

    MinHeap tempHeap = eventHeap;
    while (tempHeap.getSize() > 0 && (int)result.size() < count) {
        result.push_back(tempHeap.extractMin());
    }
    return result;
}

std::string CalendarSystem::displayStructures() const {
    std::string s;

    s += "\n1. HASH TABLE (Events by ID):\n";
    s += "   -----------------------------------------\n";
    s += eventMap.displayStatus();

    s += "\n\n2. BINARY SEARCH TREE (Dates):\n";
    s += "   -----------------------------------------\n";
    if (!eventTree.isEmpty()) {
        s += eventTree.displayDates();
    } else {
        s += "   BST is Empty\n";
    }

    s += "\n\n3. Min-Heap Priority Queue (for upcoming events):\n";
    s += "   -----------------------------------------\n";
    s += "   Heap Size: " + std::to_string(eventHeap.getSize()) + " events\n";
    s += "   Next Upcoming Event:\n";
    if (!eventHeap.isEmpty()) {
        Event next = eventHeap.peek();
        s += "   * " + next.date + " " + next.startTime + " - " + next.title + "\n";
    }

    return s;
}

bool CalendarSystem::deleteEvent(const std::string& id) {
    Event* eventToDelete = eventMap.search(id);
    if (!eventToDelete) return false;

    std::string date = eventToDelete->date;

    BSTNode* dateNode = eventTree.search(date);
    bool bstDeleted = dateNode ? dateNode->events.removeByID(id) : false;
    bool heapDeleted = eventHeap.remove(id);
    bool hashDeleted = eventMap.remove(id);

    return bstDeleted && heapDeleted && hashDeleted;
}

bool CalendarSystem::updateEvent(const std::string& id, const std::string& newTitle,
                                  const std::string& newDate, const std::string& newTime,
                                  int newDuration, int newPriority, const std::string& newDesc) {
    Event* existing = eventMap.search(id);
    if (!existing) return false;

    std::string oldDate = existing->date;
    std::string oldTime = existing->startTime;

    if (newDate != oldDate || newTime != oldTime) {
        BSTNode* dateNode = eventTree.search(newDate);
        if (dateNode) {
            Event temp;
            temp.date = newDate;
            temp.startTime = newTime;
            temp.durationMins = newDuration;
            temp.id = id;
            std::string endTime = temp.getEndTime();
            Event* conflict = dateNode->events.checkConflict(newTime, endTime);
            if (conflict && conflict->id != id) return false;
        }

        bool deleted = deleteEvent(id);
        if (!deleted) return false;

        Event newEvent;
        newEvent.id = id;
        newEvent.title = newTitle;
        newEvent.date = newDate;
        newEvent.startTime = newTime;
        newEvent.durationMins = newDuration;
        newEvent.priority = newPriority;
        newEvent.description = newDesc;

        eventTree.insert(newDate, newEvent);
        eventMap.insert(newEvent);
        eventHeap.insert(newEvent);
    } else {
        existing->title = newTitle;
        existing->durationMins = newDuration;
        existing->priority = newPriority;
        existing->description = newDesc;

        BSTNode* dateNode = eventTree.search(oldDate);
        if (dateNode) {
            ListNode* curr = dateNode->events.getHead();
            while (curr) {
                if (curr->data.id == id) {
                    curr->data.title = newTitle;
                    curr->data.durationMins = newDuration;
                    curr->data.priority = newPriority;
                    curr->data.description = newDesc;
                    break;
                }
                curr = curr->next.get();
            }
        }

        eventHeap.remove(id);
        eventHeap.insert(*existing);
    }

    return true;
}

bool CalendarSystem::exportFormattedTXT(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false;

    outFile << "-----------------------------------------------------------\n";
    outFile << "                  CALENDAR EXPORT                        \n";
    outFile << "-----------------------------------------------------------\n\n";

    std::string lastDate;
    eventTree.forEach([&](const Event& e) {
        if (e.date != lastDate) {
            if (!lastDate.empty()) outFile << "\n";
            outFile << "DATE: " << e.date << "\n";
            outFile << "-----------------------------------------------------------\n";
            lastDate = e.date;
        }
        outFile << "  +----------------- EVENT ---------------------+\n";
        outFile << "  | Title    : " << std::left << std::setw(30) << e.title << " |\n";
        outFile << "  | ID       : " << std::left << std::setw(30) << e.id << " |\n";
        outFile << "  | Time     : " << std::left << std::setw(30)
                << (e.startTime + " - " + e.getEndTime()) << " |\n";
        outFile << "  | Duration : " << std::left << std::setw(30)
                << (std::to_string(e.durationMins) + " minutes") << " |\n";
        outFile << "  | Priority : " << std::left << std::setw(30)
                << std::to_string(e.priority) << " |\n";
        if (!e.description.empty()) {
            outFile << "  | Descrip. : " << std::left << std::setw(30) << e.description << " |\n";
        }
        outFile << "  +-----------------------------------------------+\n";
    });

    outFile.close();
    return true;
}

void CalendarSystem::clearAllEvents() {
    eventTree = BST();
    eventMap.clear();
    eventHeap.clear();
    eventCounter = 0;
}

void CalendarSystem::forEachEvent(std::function<void(const Event&)> callback) const {
    eventTree.forEach(callback);
}
