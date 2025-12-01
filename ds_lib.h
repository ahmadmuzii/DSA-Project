#ifndef DS_LIB_H
#define DS_LIB_H

#include "event.h"
#include <iostream>
#include <iomanip>

// ==========================================
// 1. LINKED LIST (For Collision Chaining & Events in a Day)
// ==========================================
struct ListNode {
    Event data;
    ListNode* next;

    ListNode(Event e) : data(e), next(nullptr) {}
};

class LinkedList {
public:
    ListNode* head;

    LinkedList() : head(nullptr) {}

    // Insert sorted by time (for daily view)
    void insertSorted(Event e) {
        ListNode* newNode = new ListNode(e);
        if (!head || head->data.startTime >= e.startTime) {
            newNode->next = head;
            head = newNode;
            return;
        }
        ListNode* current = head;
        while (current->next && current->next->data.startTime < e.startTime) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    // Insert at end (for Hash Map chains)
    void append(Event e) {
        ListNode* newNode = new ListNode(e);
        if (!head) {
            head = newNode;
            return;
        }
        ListNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }

    // Check time conflict
    // Returns pointer to conflicting event if found, else nullptr
    Event* checkConflict(string start, string end) {
        ListNode* curr = head;
        while (curr) {
            string evtEnd = curr->data.getEndTime();
            // Logic: (StartA < EndB) and (EndA > StartB)
            if (start < evtEnd && end > curr->data.startTime) {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }
};

// ==========================================
// 2. BINARY SEARCH TREE (Organizes Dates)
// ==========================================
struct BSTNode {
    string date;
    LinkedList events; // List of events for this specific date
    BSTNode *left, *right;

    BSTNode(string d) : date(d), left(nullptr), right(nullptr) {}
};

class BST {
public:
    BSTNode* root;

    BST() : root(nullptr) {}

    void insert(string date, Event e) {
        root = insertRec(root, date, e);
    }

    BSTNode* search(string date) {
        return searchRec(root, date);
    }
    
    // In-order traversal to list all dates
    void displayDates(BSTNode* node) {
        if (!node) return;
        displayDates(node->left);
        cout << "Date: " << node->date << " (" << countEvents(node->events.head) << " events)" << endl;
        displayDates(node->right);
    }

private:
    BSTNode* insertRec(BSTNode* node, string date, Event e) {
        if (!node) {
            BSTNode* newNode = new BSTNode(date);
            newNode->events.insertSorted(e);
            return newNode;
        }
        if (date < node->date)
            node->left = insertRec(node->left, date, e);
        else if (date > node->date)
            node->right = insertRec(node->right, date, e);
        else {
            // Date exists, just add event to the list
            node->events.insertSorted(e);
        }
        return node;
    }

    BSTNode* searchRec(BSTNode* node, string date) {
        if (!node || node->date == date) return node;
        if (date < node->date) return searchRec(node->left, date);
        return searchRec(node->right, date);
    }

    int countEvents(ListNode* head) {
        int c = 0;
        while(head) { c++; head = head->next; }
        return c;
    }
};

// ==========================================
// 3. MIN HEAP (Priority Queue for Upcoming Events)
// ==========================================
class MinHeap {
    Event* arr; // Dynamic array
    int capacity;
    int size;

public:
    MinHeap(int cap = 100) : capacity(cap), size(0) {
        arr = new Event[capacity];
    }

    ~MinHeap() { delete[] arr; }

    int parent(int i) { return (i - 1) / 2; }
    int left(int i) { return (2 * i + 1); }
    int right(int i) { return (2 * i + 2); }

    // Sort criteria: Date first, then Time
    bool isSmaller(Event a, Event b) {
        if (a.date != b.date) return a.date < b.date;
        return a.startTime < b.startTime;
    }

    void insert(Event e) {
        if (size == capacity) {
            // Simple resize logic (doubling)
            Event* newArr = new Event[capacity * 2];
            for(int i=0; i<size; i++) newArr[i] = arr[i];
            delete[] arr;
            arr = newArr;
            capacity *= 2;
        }

        size++;
        int i = size - 1;
        arr[i] = e;

        while (i != 0 && isSmaller(arr[i], arr[parent(i)])) {
            Event temp = arr[i];
            arr[i] = arr[parent(i)];
            arr[parent(i)] = temp;
            i = parent(i);
        }
    }

    void displayTop(int k) {
        // Since this is a min-heap array, the array is not fully sorted,
        // but the top is the minimum. For perfect "Next 7 days" view, 
        // we'd usually extract min. For visualization, we just show the array 
        // or extract temporally (copying heap).
        // Here we just print the heap structure for the "View" requirement.
        
        // Better: Make a copy to extract without deleting
        MinHeap tempHeap(size + 1);
        for(int i=0; i<size; i++) tempHeap.insert(arr[i]);

        cout << "--- Nearest Upcoming Events ---\n";
        int count = 0;
        while(tempHeap.size > 0 && count < k) {
            Event e = tempHeap.extractMin();
            cout << "• " << e.date << " " << e.startTime << " - " << e.title 
                 << " (Priority: " << e.priority << ")" << endl;
            count++;
        }
    }

    Event extractMin() {
        if (size <= 0) return Event();
        if (size == 1) {
            size--;
            return arr[0];
        }

        Event root = arr[0];
        arr[0] = arr[size - 1];
        size--;
        MinHeapify(0);
        return root;
    }

    void MinHeapify(int i) {
        int l = left(i);
        int r = right(i);
        int smallest = i;

        if (l < size && isSmaller(arr[l], arr[smallest])) smallest = l;
        if (r < size && isSmaller(arr[r], arr[smallest])) smallest = r;

        if (smallest != i) {
            Event temp = arr[i];
            arr[i] = arr[smallest];
            arr[smallest] = temp;
            MinHeapify(smallest);
        }
    }
    
    int getSize() { return size; }
};

// ==========================================
// 4. HASH TABLE (For ID Search)
// ==========================================
class HashTable {
    static const int TABLE_SIZE = 10;
    LinkedList* table[TABLE_SIZE];

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++)
            table[i] = new LinkedList();
    }

    // Simple Hash Function
    int hashFunction(string key) {
        int sum = 0;
        for (char c : key) sum += c;
        return sum % TABLE_SIZE;
    }

    void insert(Event e) {
        int index = hashFunction(e.id);
        table[index]->append(e);
    }

    Event* search(string id) {
        int index = hashFunction(id);
        ListNode* curr = table[index]->head;
        while (curr) {
            if (curr->data.id == id) return &(curr->data);
            curr = curr->next;
        }
        return nullptr;
    }

    void displayStatus() {
        for(int i=0; i<TABLE_SIZE; i++) {
            cout << "[" << i << "]: ";
            ListNode* curr = table[i]->head;
            if(!curr) cout << "EMPTY";
            while(curr) {
                cout << curr->data.id << " -> ";
                curr = curr->next;
            }
            cout << endl;
        }
    }
};

#endif