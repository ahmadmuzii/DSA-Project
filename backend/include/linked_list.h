#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "event.h"
#include <memory>
#include <string>

struct ListNode {
    Event data;
    std::unique_ptr<ListNode> next;

    ListNode(Event e) : data(std::move(e)), next(nullptr) {}
};

class LinkedList {
    std::unique_ptr<ListNode> head;

public:
    LinkedList() = default;
    LinkedList(const LinkedList& other);
    LinkedList& operator=(const LinkedList& other);
    LinkedList(LinkedList&&) = default;
    LinkedList& operator=(LinkedList&&) = default;
    ~LinkedList() = default;

    void insertSorted(Event e);
    void append(Event e);
    bool removeByID(const std::string& id);
    Event* checkConflict(const std::string& start, const std::string& end);
    int count() const;
    void clear();
    std::string displayFormatted() const;

    ListNode* getHead() const { return head.get(); }
};

#endif
