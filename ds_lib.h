#ifndef DS_LIB_H
#define DS_LIB_H

#include "event.h"
#include <iostream>
#include <iomanip>
#include <string>

const int HASH_TABLE_SIZE = 21;
const int INITIAL_HEAP_CAPACITY = 100;

static std::string convertToComparable(const std::string& date) {
    if (date.length() != 10 || date[2] != ':' || date[5] != ':') {
        return date;
    }
    
    std::string year = date.substr(6, 4);
    std::string month = date.substr(3, 2);
    std::string day = date.substr(0, 2);
    
    return year + month + day;
}

struct ListNode {
    Event data;
    ListNode* next;

    ListNode(Event e) : data(e), next(nullptr) {}
};

class LinkedList {
public:
    ListNode* head;

    LinkedList() : head(nullptr) {}
    
    LinkedList(const LinkedList& other) : head(nullptr) {
        ListNode* curr = other.head;
        while (curr) {
            append(curr->data);
            curr = curr->next;
        }
    }
    
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            while (head) {
                ListNode* temp = head;
                head = head->next;
                delete temp;
            }
            
            ListNode* curr = other.head;
            while (curr) {
                append(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }
    
    ~LinkedList() {
        while (head) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insertSorted(Event e) {
        ListNode* newNode = new ListNode(e);
        
        if (!head) {
            head = newNode;
            return;
        }
        
        if (e.startTime < head->data.startTime) {
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

    void append(Event e) {
        ListNode* newNode = new ListNode(e);
        if (!head) {
            head = newNode;
            return;
        }
        ListNode* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

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
    
    void displayFormatted() {
        ListNode* curr = head;
        int count = 1;
        while (curr) {
            cout << "\n  +--------------- EVENT #" << count << " ----------------+\n";
            cout << "  | Title    : " << left << setw(30) << curr->data.title << " |\n";
            cout << "  | ID       : " << left << setw(30) << curr->data.id << " |\n";
            cout << "  | Time     : " << left << setw(30) << 
                 (curr->data.startTime + " - " + curr->data.getEndTime()) << " |\n";
            cout << "  | Priority : " << left << setw(30) << 
                 to_string(curr->data.priority) << " |\n";
            
            // ADDED: Display description if it exists
            if (!curr->data.description.empty()) {
                cout << "  | Descrip. : " << left << setw(30) << curr->data.description << " |\n";
            }
            
            cout << "  +-----------------------------------------------+\n";
            curr = curr->next;
            count++;
        }
    }
};

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
    
    BST(const BST& other) : root(nullptr) {
        copyTree(other.root);
    }
    
    BST& operator=(const BST& other) {
        if (this != &other) {
            clearTree(root);
            root = nullptr;
            copyTree(other.root);
        }
        return *this;
    }
    
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
        if (!node) {
            return;
        }
        displayDates(node->left);
        cout << "   * " << node->date << " (" << countEvents(node->events.head) << " events)\n";
        displayDates(node->right);
    }
    
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
        
        string date1 = convertToComparable(date);
        string date2 = convertToComparable(node->date);
        
        if (date1 < date2) {
            node->left = insertRec(node->left, date, e);
        } else if (date1 > date2) {
            node->right = insertRec(node->right, date, e);
        } else {
            node->events.insertSorted(e);
        }
        return node;
    }

    BSTNode* searchRec(BSTNode* node, string date) {
        if (!node) {
            return nullptr;
        }
        
        string date1 = convertToComparable(date);
        string date2 = convertToComparable(node->date);
        
        if (date1 == date2) {
            return node;
        }
        if (date1 < date2) {
            return searchRec(node->left, date);
        }
        return searchRec(node->right, date);
    }

    int countEvents(ListNode* head) {
        int c = 0;
        while (head) { 
            c++; 
            head = head->next; 
        }
        return c;
    }
    
    void copyTree(BSTNode* node) {
        if (!node) {
            return;
        }
        
        ListNode* curr = node->events.head;
        while (curr) {
            insert(node->date, curr->data);
            curr = curr->next;
        }
        
        copyTree(node->left);
        copyTree(node->right);
    }
    
    void clearTree(BSTNode* node) {
        if (!node) {
            return;
        }
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
};

class MinHeap {
    Event* arr;
    int capacity;
    int size;

public:
    MinHeap(int cap = INITIAL_HEAP_CAPACITY) : capacity(cap), size(0) {
        arr = new Event[capacity];
    }
    
    MinHeap(const MinHeap& other) : capacity(other.capacity), size(other.size) {
        arr = new Event[capacity];
        for (int i = 0; i < size; i++) {
            arr[i] = other.arr[i];
        }
    }
    
    MinHeap& operator=(const MinHeap& other) {
        if (this != &other) {
            delete[] arr;
            capacity = other.capacity;
            size = other.size;
            arr = new Event[capacity];
            for (int i = 0; i < size; i++) {
                arr[i] = other.arr[i];
            }
        }
        return *this;
    }
    
    ~MinHeap() { 
        delete[] arr; 
    }

    int parent(int i) { 
        return (i - 1) / 2; 
    }
    
    int left(int i) { 
        return (2 * i + 1); 
    }
    
    int right(int i) { 
        return (2 * i + 2); 
    }

    bool isSmaller(Event a, Event b) {
        string dateA = convertToComparable(a.date);
        string dateB = convertToComparable(b.date);
        
        if (dateA != dateB) { 
            return dateA < dateB;
        }
        return a.startTime < b.startTime;
    }

    void insert(Event e) {
        if (size == capacity) {
            capacity *= 2;
            Event* newArr = new Event[capacity];
            for (int i = 0; i < size; i++) {
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
        if (size == 0) {
            cout << "\n   No upcoming events\n";
            return;
        }
        
        MinHeap tempHeap(*this);
        
        cout << "\nNext Upcoming Events:\n";
        cout << "--------------------------------------------------------------\n";
        
        int count = 0;
        while (tempHeap.size > 0 && count < k) {
            Event e = tempHeap.extractMin();
            cout << "\n  * " << e.date << " | " << e.startTime << " - " << e.getEndTime() << "\n";
            cout << "    " << e.title << " (Priority: " << e.priority << ", ID: " << e.id << ")\n";
            
            // Display description if it exists
            if (!e.description.empty()) {
                cout << "    Description: " << e.description << "\n";
            }
            
            count++;
        }
        
        if (count == 0) {
            cout << "   No upcoming events\n";
        }
    }

    Event extractMin() {
        if (size <= 0) {
            return Event();
        }
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

        if (l < size && isSmaller(arr[l], arr[smallest])) {
            smallest = l;
        }
        if (r < size && isSmaller(arr[r], arr[smallest])) {
            smallest = r;
        }

        if (smallest != i) {
            Event temp = arr[i];
            arr[i] = arr[smallest];
            arr[smallest] = temp;
            MinHeapify(smallest);
        }
    }
    
    int getSize() { 
        return size; 
    }
    
    void clear() {
        size = 0;
    }
    
    bool isEmpty() { 
        return size == 0; 
    }
};

class HashTable {
    static const int TABLE_SIZE = HASH_TABLE_SIZE;
    LinkedList* table[TABLE_SIZE];

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = new LinkedList();
        }
    }
    
    HashTable(const HashTable& other) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = new LinkedList(*(other.table[i]));
        }
    }
    
    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            for (int i = 0; i < TABLE_SIZE; i++) {
                delete table[i];
                table[i] = new LinkedList(*(other.table[i]));
            }
        }
        return *this;
    }
    
    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            delete table[i];
        }
    }

    int hashFunction(string key) {
        // DJB2 hash algorithm 
        unsigned long hash = 5381;
        for (char c : key) {
            hash = hash * 33 + c; 
        }
        return hash % TABLE_SIZE;
    }

    void insert(Event e) {
        int index = hashFunction(e.id);
        table[index]->append(e);
    }

    Event* search(string id) {
        int index = hashFunction(id);
        ListNode* curr = table[index]->head;
        while (curr) {
            if (curr->data.id == id) {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    void displayStatus() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << "   [" << i << "]: ";
            ListNode* curr = table[i]->head;
            if (!curr) {
                cout << "EMPTY";
            } else {
                int count = 0;
                while (curr) {
                    if (count > 0) {
                        cout << " -> ";
                    }
                    cout << curr->data.id;
                    curr = curr->next;
                    count++;
                }
                cout << " (" << count << " events)";
            }
            cout << endl;
        }
    }
    
    LinkedList** getTable() { 
        return table; 
    }
    
    void clear() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            delete table[i];
            table[i] = new LinkedList();
        }
    }
};

#endif