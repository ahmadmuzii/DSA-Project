#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "event.h"
#include <vector>
#include <unordered_map>
#include <string>

class MinHeap {
    std::vector<Event> arr;
    std::unordered_map<std::string, int> posMap;

    int parent(int i) const { return (i - 1) / 2; }
    int left(int i) const { return 2 * i + 1; }
    int right(int i) const { return 2 * i + 2; }
    bool isSmaller(const Event& a, const Event& b) const;
    void swap(int i, int j);
    void siftUp(int i);
    void siftDown(int i);

public:
    MinHeap() = default;

    void insert(Event e);
    Event extractMin();
    Event peek() const;
    bool remove(const std::string& id);
    int getSize() const { return (int)arr.size(); }
    bool isEmpty() const { return arr.empty(); }
    void clear();
};

#endif
