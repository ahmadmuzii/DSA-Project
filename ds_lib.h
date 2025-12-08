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
    
    // Copy constructor
    LinkedList(const LinkedList& other) : head(nullptr) {
        ListNode* curr = other.head;
        while(curr) {
            append(curr->data);
            curr = curr->next;
        }
    }
    
    // Assignment operator
    LinkedList& operator=(const LinkedList& other) {
        if(this != &other) {
            // Clear current list
            while(head) {
                ListNode* temp = head;
                head = head->next;
                delete temp;
            }
            
            // Copy from other
            ListNode* curr = other.head;
            while(curr) {
                append(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }
    
    // Destructor
    ~LinkedList() {
        while(head) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

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
    Event* checkConflict(string start, string end) {
        ListNode* curr = head;
        while (curr) {
            string evtEnd = curr->data.getEndTime();
            if (start < evtEnd && end > curr->data.startTime) {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }
    
    // Get formatted display
    void displayFormatted() {
        ListNode* curr = head;
        int count = 1;
        while(curr) {
            cout << "\n  +--------------- EVENT #" << count << " ----------------+\n";
            cout << "  | Title    : " << left << setw(30) << curr->data.title << " |\n";
            cout << "  | ID       : " << left << setw(30) << curr->data.id << " |\n";
            cout << "  | Time     : " << left << setw(30) << 
                 (curr->data.startTime + " - " + curr->data.getEndTime()) << " |\n";
            cout << "  | Priority : " << left << setw(30) << 
                 to_string(curr->data.priority) << " |\n";
            cout << "  +-----------------------------------------------+\n";
            curr = curr->next;
            count++;
        }
    }
};

// ==========================================
// 2. BINARY SEARCH TREE (Organizes Dates)
// ==========================================
struct BSTNode {
    string date;
    LinkedList events;
    BSTNode *left, *right;

    BSTNode(string d) : date(d), left(nullptr), right(nullptr) {}
};

class BST {
public:
    BSTNode* root;

    BST() : root(nullptr) {}
    
    // Copy constructor
    BST(const BST& other) : root(nullptr) {
        copyTree(other.root);
    }
    
    // Assignment operator
    BST& operator=(const BST& other) {
        if(this != &other) {
            clearTree(root);
            root = nullptr;
            copyTree(other.root);
        }
        return *this;
    }
    
    // Destructor
    ~BST() {
        clearTree(root);
    }

    void insert(string date, Event e) {
        root = insertRec(root, date, e);
    }

    BSTNode* search(string date) {
        return searchRec(root, date);
    }
    
    void displayDates(BSTNode* node) {
        if (!node) return;
        displayDates(node->left);
        cout << "   * " << node->date << " (" << countEvents(node->events.head) << " events)\n";
        displayDates(node->right);
    }
    
    // Get events for a date (formatted)
    void displayEventsByDate(string date) {
        BSTNode* node = search(date);
        if (!node) {
            cout << "   No events for " << date << "\n";
            return;
        }
        node->events.displayFormatted();
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
    
    void copyTree(BSTNode* node) {
        if(!node) return;
        
        // Copy events from this node
        ListNode* curr = node->events.head;
        while(curr) {
            insert(node->date, curr->data);
            curr = curr->next;
        }
        
        copyTree(node->left);
        copyTree(node->right);
    }
    
    void clearTree(BSTNode* node) {
        if(!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
};

// ==========================================
// 3. MIN HEAP (Priority Queue for Upcoming Events)
// ==========================================
class MinHeap {
    Event* arr;
    int capacity;
    int size;

public:
    // Constructor
    MinHeap(int cap = 100) : capacity(cap), size(0) {
        arr = new Event[capacity];
    }
    
    // Copy constructor
    MinHeap(const MinHeap& other) : capacity(other.capacity), size(other.size) {
        arr = new Event[capacity];
        for(int i = 0; i < size; i++) {
            arr[i] = other.arr[i];
        }
    }
    
    // Assignment operator
    MinHeap& operator=(const MinHeap& other) {
        if (this != &other) {
            delete[] arr;
            capacity = other.capacity;
            size = other.size;
            arr = new Event[capacity];
            for(int i = 0; i < size; i++) {
                arr[i] = other.arr[i];
            }
        }
        return *this;
    }
    
    // Destructor
    ~MinHeap() { 
        delete[] arr; 
    }

    int parent(int i) { return (i - 1) / 2; }
    int left(int i) { return (2 * i + 1); }
    int right(int i) { return (2 * i + 2); }

    bool isSmaller(Event a, Event b) {
        if (a.date != b.date) return a.date < b.date;
        return a.startTime < b.startTime;
    }

    void insert(Event e) {
        if (size == capacity) {
            capacity *= 2;
            Event* newArr = new Event[capacity];
            for(int i = 0; i < size; i++) {
                newArr[i] = arr[i];
            }
            delete[] arr;
            arr = newArr;
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
        if(size == 0) {
            cout << "\n   No upcoming events\n";
            return;
        }
        
        // Create a copy to extract without affecting the original
        MinHeap tempHeap(*this);
        
        cout << "\nNext " << k << " Upcoming Events:\n";
        cout << "--------------------------------------------------------------\n";
        
        int count = 0;
        while(tempHeap.size > 0 && count < k) {
            Event e = tempHeap.extractMin();
            cout << "\n  * " << e.date << " | " << e.startTime << " - " << e.getEndTime() << "\n";
            cout << "    " << e.title << " (Priority: " << e.priority << ", ID: " << e.id << ")\n";
            count++;
        }
        
        if(count == 0) {
            cout << "   No upcoming events\n";
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
    
    // Clear all events
    void clear() {
        size = 0;
    }
    
    // Check if empty
    bool isEmpty() { return size == 0; }
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
    
    // Copy constructor
    HashTable(const HashTable& other) {
        for(int i = 0; i < TABLE_SIZE; i++) {
            table[i] = new LinkedList(*(other.table[i]));
        }
    }
    
    // Assignment operator
    HashTable& operator=(const HashTable& other) {
        if(this != &other) {
            for(int i = 0; i < TABLE_SIZE; i++) {
                delete table[i];
                table[i] = new LinkedList(*(other.table[i]));
            }
        }
        return *this;
    }
    
    // Destructor
    ~HashTable() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            delete table[i];
        }
    }

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
        for(int i = 0; i < TABLE_SIZE; i++) {
            cout << "   [" << i << "]: ";
            ListNode* curr = table[i]->head;
            if(!curr) {
                cout << "EMPTY";
            } else {
                int count = 0;
                while(curr) {
                    if(count > 0) cout << " -> ";
                    cout << curr->data.id;
                    curr = curr->next;
                    count++;
                }
                cout << " (" << count << " events)";
            }
            cout << endl;
        }
    }
    
    // Get table for deletion operations
    LinkedList** getTable() { return table; }
    
    // Clear all events
    void clear() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            delete table[i];
            table[i] = new LinkedList();
        }
    }
};

#endif