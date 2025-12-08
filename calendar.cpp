#include "calendar.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

// Generate ID: EVT_12345
string generateID(int& counter) {
    counter++;
    return "EVT_" + to_string(12340 + counter);
}

// Forward declarations for helper functions
BSTNode* findDateNodeRec(BSTNode* node, string id);
void exportRecursive(BSTNode* node, ofstream& file);
void searchHelper(BSTNode* node, string query, int& matches);

// ================== HELPER FUNCTION IMPLEMENTATIONS ==================

// Helper to find date node by event ID
BSTNode* findDateNodeRec(BSTNode* node, string id) {
    if(!node) return nullptr;
    
    ListNode* curr = node->events.head;
    while(curr) {
        if(curr->data.id == id) return node;
        curr = curr->next;
    }
    
    BSTNode* leftResult = findDateNodeRec(node->left, id);
    if(leftResult) return leftResult;
    
    return findDateNodeRec(node->right, id);
}

// Helper for recursive export
void exportRecursive(BSTNode* node, ofstream& file) {
    if (!node) return;
    exportRecursive(node->left, file);
    
    file << "DATE: " << node->date << "\n";
    file << "==============================================================\n";
    
    ListNode* curr = node->events.head;
    int count = 1;
    while(curr) {
        file << "\n  +--------------- EVENT #" << count << " ----------------+\n";
        file << "  | Title    : " << left << setw(30) << curr->data.title << " |\n";
        file << "  | ID       : " << left << setw(30) << curr->data.id << " |\n";
        file << "  | Time     : " << left << setw(30) << 
             (curr->data.startTime + " - " + curr->data.getEndTime()) << " |\n";
        file << "  | Duration : " << left << setw(30) << 
             (to_string(curr->data.durationMins) + " minutes") << " |\n";
        file << "  | Priority : " << left << setw(30) << 
             to_string(curr->data.priority) << " |\n";
        file << "  +-----------------------------------------------+\n";
        curr = curr->next;
        count++;
    }
    file << "\n";
    
    exportRecursive(node->right, file);
}

// Search Helper
void searchHelper(BSTNode* node, string query, int& matches) {
    if(!node) return;
    
    ListNode* curr = node->events.head;
    while(curr) {
        if (curr->data.title.find(query) != string::npos || 
            curr->data.id.find(query) != string::npos) {
            cout << "\n* " << curr->data.title << "\n";
            cout << "  Date: " << curr->data.date << " | Time: " 
                 << curr->data.startTime << " | ID: " << curr->data.id << "\n";
            matches++;
        }
        curr = curr->next;
    }
    
    searchHelper(node->left, query, matches);
    searchHelper(node->right, query, matches);
}

// ================== CALENDARSYSTEM CLASS IMPLEMENTATIONS ==================

bool CalendarSystem::addEvent(string title, string date, string start, int duration, int priority, string desc) {
    
    // 1. Create temporary Event object to check time
    Event newEvent;
    newEvent.title = title;
    newEvent.date = date;
    newEvent.startTime = start;
    newEvent.durationMins = duration;
    newEvent.priority = priority;
    
    string endTime = newEvent.getEndTime();

    // 2. Check Conflicts in BST
    BSTNode* dateNode = eventTree.search(date);
    if (dateNode) {
        Event* conflict = dateNode->events.checkConflict(start, endTime);
        if (conflict) {
            cout << "\n==============================================================\n";
            cout << "                    CONFLICT DETECTED!                    \n";
            cout << "==============================================================\n";
            cout << "\n[WARNING] Event conflicts with:\n";
            cout << "   * " << conflict->title << "\n";
            cout << "   * Time: " << conflict->startTime << " - " << conflict->getEndTime() << "\n";
            cout << "   * ID: " << conflict->id << "\n";
            return false;
        }
    }

    // 3. Finalize Event Creation
    newEvent.id = generateID(eventCounter);
    
    // 4. Insert into ALL Data Structures
    eventTree.insert(date, newEvent);
    eventMap.insert(newEvent);
    eventHeap.insert(newEvent);

    cout << "\n==============================================================\n";
    cout << "                    EVENT ADDED SUCCESSFULLY!             \n";
    cout << "==============================================================\n";
    cout << "\nEvent Details:\n";
    cout << "   * ID: " << newEvent.id << "\n";
    cout << "   * Title: " << title << "\n";
    cout << "   * Date: " << date << "\n";
    cout << "   * Time: " << start << " - " << endTime << "\n";
    cout << "   * Hash Table Position: bucket_" << eventMap.hashFunction(newEvent.id) << "\n";
    return true;
}

// Used for loading from file (skips ID generation)
void CalendarSystem::addEventDirect(Event e) {
    // Check if event already exists before adding
    if (eventMap.search(e.id)) {
        cout << "[INFO] Event " << e.id << " already exists, skipping duplicate.\n";
        return;
    }
    
    eventTree.insert(e.date, e);
    eventMap.insert(e);
    eventHeap.insert(e);
    
    // Update counter to maintain ID uniqueness
    if (e.id.find("EVT_") != string::npos) {
        string numStr = e.id.substr(4);
        int num = stoi(numStr);
        if (num >= 12341 && num > eventCounter) {
            eventCounter = num - 12340;
        }
    }
}

void CalendarSystem::viewEventsByDate(string date) {
    cout << "\n==============================================================\n";
    cout << "                  EVENTS FOR " << left << setw(30) << date << "\n";
    cout << "==============================================================\n";
    
    BSTNode* node = eventTree.search(date);
    
    if (!node) {
        cout << "\nNo events found for " << date << "\n";
        return;
    }

    ListNode* curr = node->events.head;
    int count = 1;
    while(curr) {
        cout << "\n+----------------- EVENT #" << count << " -----------------+\n";
        cout << "| Title    : " << left << setw(38) << curr->data.title << " |\n";
        cout << "| ID       : " << left << setw(38) << curr->data.id << " |\n";
        cout << "| Time     : " << left << setw(38) << 
             (curr->data.startTime + " - " + curr->data.getEndTime()) << " |\n";
        cout << "| Duration : " << left << setw(38) << 
             (to_string(curr->data.durationMins) + " minutes") << " |\n";
        cout << "| Priority : " << left << setw(38) << 
             (to_string(curr->data.priority) + " (" + 
             (curr->data.priority == 1 ? "Highest" : 
              curr->data.priority == 2 ? "High" :
              curr->data.priority == 3 ? "Medium" :
              curr->data.priority == 4 ? "Low" : "Lowest") + ")") << " |\n";
        cout << "+-----------------------------------------------------+\n";
        curr = curr->next;
        count++;
    }
    cout << "\nTotal Events: " << (count-1) << "\n";
}

void CalendarSystem::searchEvents(string query) {
    cout << "\n==============================================================\n";
    cout << "          SEARCH RESULTS FOR: \"" << left << setw(30) << query << "\"\n";
    cout << "==============================================================\n";
    
    int matches = 0;
    searchHelper(eventTree.root, query, matches);
    
    if(matches == 0) {
        cout << "\nNo matches found for \"" << query << "\"\n";
    } else {
        cout << "\nFound " << matches << " match(es)\n";
    }
}

void CalendarSystem::checkConflicts(string date) {
    cout << "\n==============================================================\n";
    cout << "                  CHECKING TIME CONFLICTS                 \n";
    cout << "==============================================================\n";
    
    BSTNode* node = eventTree.search(date);
    if(!node) {
        cout << "\nNo events scheduled for " << date << "\n";
        return;
    }
    
    // Check for conflicts manually
    ListNode* curr = node->events.head;
    bool foundConflict = false;
    int eventCount = 0;
    
    while(curr) {
        eventCount++;
        ListNode* nextEvent = curr->next;
        while(nextEvent) {
            string currEnd = curr->data.getEndTime();
            string nextEnd = nextEvent->data.getEndTime();
            
            if(curr->data.startTime < nextEnd && currEnd > nextEvent->data.startTime) {
                if(!foundConflict) {
                    cout << "\n[CONFLICTS FOUND]:\n";
                    foundConflict = true;
                }
                cout << "\nConflict between:\n";
                cout << "   * " << curr->data.title << " (" << curr->data.startTime << "-" << currEnd << ")\n";
                cout << "   * " << nextEvent->data.title << " (" << nextEvent->data.startTime << "-" << nextEnd << ")\n";
            }
            nextEvent = nextEvent->next;
        }
        curr = curr->next;
    }
    
    if(!foundConflict) {
        cout << "\nNo overlapping events found for " << date << "\n";
        cout << "Total events scheduled: " << eventCount << "\n";
    }
}

void CalendarSystem::viewUpcoming() {
    cout << "\n==============================================================\n";
    cout << "                  UPCOMING EVENTS (Next 7)               \n";
    cout << "==============================================================\n";
    eventHeap.displayTop(7);
}

void CalendarSystem::displayStructures() {
    cout << "\n==============================================================\n";
    cout << "              DATA STRUCTURES STATUS                     \n";
    cout << "==============================================================\n";
    
    cout << "\n1. HASH TABLE (Events by ID):\n";
    cout << "   -----------------------------------------\n";
    eventMap.displayStatus();
    
    cout << "\n\n2. BINARY SEARCH TREE (Dates):\n";
    cout << "   -----------------------------------------\n";
    if(eventTree.root) {
        cout << "   Root Node Date: " << eventTree.root->date << "\n";
        cout << "   Tree Structure (In-order):\n";
        eventTree.displayDates(eventTree.root);
    } else {
        cout << "   BST is Empty\n";
    }
    
    cout << "\n\n3. PRIORITY QUEUE (Min-Heap):\n";
    cout << "   -----------------------------------------\n";
    cout << "   Heap Size: " << eventHeap.getSize() << " events\n";
    cout << "   Next Upcoming Event:\n";
    if(eventHeap.getSize() > 0) {
        MinHeap tempHeap = eventHeap;
        Event next = tempHeap.extractMin();
        cout << "   * " << next.date << " " << next.startTime << " - " << next.title << "\n";
    }
}

// Helper to find date node by event ID
BSTNode* CalendarSystem::findDateNodeByEventId(string id) {
    return findDateNodeRec(eventTree.root, id);
}

bool CalendarSystem::deleteFromBST(string date, string id) {
    BSTNode* dateNode = eventTree.search(date);
    if(!dateNode) return false;
    
    ListNode* curr = dateNode->events.head;
    ListNode* prev = nullptr;
    
    // Find and remove from linked list
    while(curr) {
        if(curr->data.id == id) {
            if(prev) {
                prev->next = curr->next;
            } else {
                dateNode->events.head = curr->next;
            }
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

bool CalendarSystem::deleteFromHeap(string id) {
    // Since heap doesn't support direct deletion by ID,
    // we need to rebuild it without the deleted event
    MinHeap newHeap(eventHeap.getSize());
    bool found = false;
    
    // Create a copy of the heap
    vector<Event> events;
    MinHeap tempHeap = eventHeap;
    while(tempHeap.getSize() > 0) {
        Event e = tempHeap.extractMin();
        events.push_back(e);
    }
    
    // Rebuild without the deleted event
    for(Event e : events) {
        if(e.id != id) {
            newHeap.insert(e);
        } else {
            found = true;
        }
    }
    
    if(found) {
        // Update the original heap
        eventHeap = newHeap;
    }
    return found;
}

bool CalendarSystem::deleteFromHash(string id) {
    int index = eventMap.hashFunction(id);
    LinkedList* list = eventMap.getTable()[index];
    ListNode* curr = list->head;
    ListNode* prev = nullptr;
    
    while(curr) {
        if(curr->data.id == id) {
            if(prev) {
                prev->next = curr->next;
            } else {
                list->head = curr->next;
            }
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

bool CalendarSystem::deleteEvent(string id) {
    cout << "\n==============================================================\n";
    cout << "                    DELETE EVENT                         \n";
    cout << "==============================================================\n";
    
    Event* eventToDelete = eventMap.search(id);
    if(!eventToDelete) {
        cout << "\n[ERROR] Event with ID \"" << id << "\" not found!\n";
        return false;
    }
    
    cout << "\nEvent to Delete:\n";
    cout << "   * Title: " << eventToDelete->title << "\n";
    cout << "   * Date: " << eventToDelete->date << "\n";
    cout << "   * Time: " << eventToDelete->startTime << "\n";
    cout << "   * ID: " << eventToDelete->id << "\n";
    
    cout << "\nAre you sure you want to delete this event? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore();
    
    if(confirm != 'y' && confirm != 'Y') {
        cout << "   Deletion cancelled.\n";
        return false;
    }
    
    // Delete from all data structures
    bool bstDeleted = deleteFromBST(eventToDelete->date, id);
    bool heapDeleted = deleteFromHeap(id);
    bool hashDeleted = deleteFromHash(id);
    
    if(bstDeleted && heapDeleted && hashDeleted) {
        cout << "\n[SUCCESS] Event deleted successfully from all data structures!\n";
        return true;
    } else {
        cout << "\n[WARNING] Event partially deleted. Some data structures may be inconsistent.\n";
        return false;
    }
}

bool CalendarSystem::updateEvent(string id) {
    cout << "\n==============================================================\n";
    cout << "                    UPDATE EVENT                         \n";
    cout << "==============================================================\n";
    
    Event* eventToUpdate = eventMap.search(id);
    if(!eventToUpdate) {
        cout << "\n[ERROR] Event with ID \"" << id << "\" not found!\n";
        return false;
    }
    
    cout << "\nCurrent Event Details:\n";
    cout << "   +-------------------------------------+\n";
    cout << "   | Title    : " << left << setw(25) << eventToUpdate->title << " |\n";
    cout << "   | Date     : " << left << setw(25) << eventToUpdate->date << " |\n";
    cout << "   | Time     : " << left << setw(25) << eventToUpdate->startTime << " |\n";
    cout << "   | Duration : " << left << setw(25) << to_string(eventToUpdate->durationMins) + " mins" << " |\n";
    cout << "   | Priority : " << left << setw(25) << to_string(eventToUpdate->priority) << " |\n";
    cout << "   +-------------------------------------+\n";
    
    cout << "\nEnter new details (press Enter to keep current value):\n";
    
    string newTitle, newDate, newTime;
    int newDuration, newPriority;
    
    cout << "   New Title [" << eventToUpdate->title << "]: ";
    getline(cin, newTitle);
    if(newTitle.empty()) newTitle = eventToUpdate->title;
    
    cout << "   New Date (YYYY-MM-DD) [" << eventToUpdate->date << "]: ";
    getline(cin, newDate);
    if(newDate.empty()) newDate = eventToUpdate->date;
    
    cout << "   New Start Time (HH:MM) [" << eventToUpdate->startTime << "]: ";
    getline(cin, newTime);
    if(newTime.empty()) newTime = eventToUpdate->startTime;
    
    cout << "   New Duration (minutes) [" << eventToUpdate->durationMins << "]: ";
    string durInput;
    getline(cin, durInput);
    newDuration = durInput.empty() ? eventToUpdate->durationMins : stoi(durInput);
    
    cout << "   New Priority (1-5) [" << eventToUpdate->priority << "]: ";
    string prioInput;
    getline(cin, prioInput);
    newPriority = prioInput.empty() ? eventToUpdate->priority : stoi(prioInput);
    
    // Check if date/time changed significantly
    if(newDate != eventToUpdate->date || newTime != eventToUpdate->startTime) {
        // Delete old event and add new one
        deleteEvent(id);
        
        Event newEvent;
        newEvent.id = id; // Keep same ID
        newEvent.title = newTitle;
        newEvent.date = newDate;
        newEvent.startTime = newTime;
        newEvent.durationMins = newDuration;
        newEvent.priority = newPriority;
        
        // Check for conflicts before adding
        BSTNode* dateNode = eventTree.search(newDate);
        if (dateNode) {
            string endTime = newEvent.getEndTime();
            Event* conflict = dateNode->events.checkConflict(newTime, endTime);
            if (conflict && conflict->id != id) { // Allow self-conflict
                cout << "\n[ERROR] Cannot update: Time conflict detected!\n";
                // Re-add old event
                addEventDirect(*eventToUpdate);
                return false;
            }
        }
        
        // Add updated event
        addEventDirect(newEvent);
    } else {
        // Just update the fields
        eventToUpdate->title = newTitle;
        eventToUpdate->durationMins = newDuration;
        eventToUpdate->priority = newPriority;
        
        // Update in heap (rebuild)
        deleteFromHeap(id);
        eventHeap.insert(*eventToUpdate);
    }
    
    cout << "\n[SUCCESS] Event updated successfully!\n";
    return true;
}

void CalendarSystem::exportFormattedTXT(string filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "[ERROR] Error opening file for export!\n";
        return;
    }
    
    outFile << "==============================================================\n";
    outFile << "                  CALENDAR EXPORT                        \n";
    outFile << "==============================================================\n\n";
    
    // Export recursively
    exportRecursive(eventTree.root, outFile);
    
    outFile.close();
    cout << "[SUCCESS] Calendar exported to " << filename << " with formatting!\n";
}

void CalendarSystem::clearAllEvents() {
    // Clear BST (simplified - in real implementation, you'd need proper memory management)
    eventTree.root = nullptr; // This is simplified
    
    // Clear Hash Table
    for(int i = 0; i < 10; i++) { // TABLE_SIZE is 10
        LinkedList* list = eventMap.getTable()[i];
        ListNode* curr = list->head;
        while(curr) {
            ListNode* toDelete = curr;
            curr = curr->next;
            delete toDelete;
        }
        list->head = nullptr;
    }
    
    // Clear Heap
    eventHeap = MinHeap(100); // Reset to empty heap
    
    eventCounter = 0;
    cout << "[INFO] All events cleared from memory.\n";
}