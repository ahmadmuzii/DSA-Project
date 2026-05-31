#include "hash_table.h"
#include <iostream>

int HashTable::hashFunction(const std::string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = hash * 33 + c;
    }
    return hash % TABLE_SIZE;
}

void HashTable::insert(const Event& e) {
    int index = hashFunction(e.id);
    table[index].append(e);
}

Event* HashTable::search(const std::string& id) const {
    int index = hashFunction(id);
    ListNode* curr = table[index].getHead();
    while (curr) {
        if (curr->data.id == id) {
            return const_cast<Event*>(&(curr->data));
        }
        curr = curr->next.get();
    }
    return nullptr;
}

bool HashTable::remove(const std::string& id) {
    int index = hashFunction(id);
    return table[index].removeByID(id);
}

std::string HashTable::displayStatus() const {
    std::string result;
    for (int i = 0; i < TABLE_SIZE; i++) {
        result += "   [" + std::to_string(i) + "]: ";
        ListNode* curr = table[i].getHead();
        if (!curr) {
            result += "EMPTY";
        } else {
            int count = 0;
            while (curr) {
                if (count > 0) result += " -> ";
                result += curr->data.id;
                curr = curr->next.get();
                count++;
            }
            result += " (" + std::to_string(count) + " events)";
        }
        result += "\n";
    }
    return result;
}

void HashTable::clear() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i].clear();
    }
}
