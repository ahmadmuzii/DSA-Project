#include "min_heap.h"
#include "ds_utility.h"

bool MinHeap::isSmaller(const Event& a, const Event& b) const {
    std::string dateA = convertToComparable(a.date);
    std::string dateB = convertToComparable(b.date);
    if (dateA != dateB) return dateA < dateB;
    return timeToMinutes(a.startTime) < timeToMinutes(b.startTime);
}

void MinHeap::swap(int i, int j) {
    std::swap(arr[i], arr[j]);
    posMap[arr[i].id] = i;
    posMap[arr[j].id] = j;
}

void MinHeap::siftUp(int i) {
    while (i > 0 && isSmaller(arr[i], arr[parent(i)])) {
        swap(i, parent(i));
        i = parent(i);
    }
}

void MinHeap::siftDown(int i) {
    int smallest = i;
    int l = left(i);
    int r = right(i);

    if (l < (int)arr.size() && isSmaller(arr[l], arr[smallest]))
        smallest = l;
    if (r < (int)arr.size() && isSmaller(arr[r], arr[smallest]))
        smallest = r;

    if (smallest != i) {
        swap(i, smallest);
        siftDown(smallest);
    }
}

void MinHeap::insert(Event e) {
    const std::string& id = e.id;
    arr.push_back(std::move(e));
    int idx = (int)arr.size() - 1;
    posMap[id] = idx;
    siftUp(idx);
}

Event MinHeap::extractMin() {
    if (arr.empty()) return Event();
    if (arr.size() == 1) {
        Event e = std::move(arr.back());
        posMap.erase(e.id);
        arr.pop_back();
        return e;
    }

    Event root = std::move(arr[0]);
    posMap.erase(root.id);

    arr[0] = std::move(arr.back());
    posMap[arr[0].id] = 0;
    arr.pop_back();

    siftDown(0);
    return root;
}

Event MinHeap::peek() const {
    if (arr.empty()) return Event();
    return arr[0];
}

bool MinHeap::remove(const std::string& id) {
    auto it = posMap.find(id);
    if (it == posMap.end()) return false;

    int idx = it->second;
    int lastIdx = (int)arr.size() - 1;

    if (idx != lastIdx) {
        arr[idx] = std::move(arr[lastIdx]);
        posMap[arr[idx].id] = idx;
        arr.pop_back();
        posMap.erase(id);

        int p = parent(idx);
        if (idx > 0 && isSmaller(arr[idx], arr[p])) {
            siftUp(idx);
        } else {
            siftDown(idx);
        }
    } else {
        arr.pop_back();
        posMap.erase(id);
    }
    return true;
}

void MinHeap::clear() {
    arr.clear();
    posMap.clear();
}
