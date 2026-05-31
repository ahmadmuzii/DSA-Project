#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"
#include <string>

class HashTable {
    static constexpr int TABLE_SIZE = 31;
    LinkedList table[TABLE_SIZE];

public:
    HashTable() = default;

    int hashFunction(const std::string& key) const;
    void insert(const Event& e);
    Event* search(const std::string& id) const;
    bool remove(const std::string& id);
    std::string displayStatus() const;
    void clear();
    int getTableSize() const { return TABLE_SIZE; }
    const LinkedList* getTable() const { return table; }
};

#endif
