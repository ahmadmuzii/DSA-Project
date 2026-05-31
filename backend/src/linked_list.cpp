#include "linked_list.h"
#include "ds_utility.h"
#include <iostream>
#include <iomanip>

LinkedList::LinkedList(const LinkedList& other) {
    ListNode* curr = other.head.get();
    ListNode** tail = &head;
    while (curr) {
        *tail = std::make_unique<ListNode>(curr->data);
        tail = &((*tail)->next);
        curr = curr->next.get();
    }
}

LinkedList& LinkedList::operator=(const LinkedList& other) {
    if (this != &other) {
        head.reset();
        ListNode* curr = other.head.get();
        ListNode** tail = &head;
        while (curr) {
            *tail = std::make_unique<ListNode>(curr->data);
            tail = &((*tail)->next);
            curr = curr->next.get();
        }
    }
    return *this;
}

void LinkedList::insertSorted(Event e) {
    auto newNode = std::make_unique<ListNode>(std::move(e));

    if (!head || newNode->data.startTime < head->data.startTime) {
        newNode->next = std::move(head);
        head = std::move(newNode);
        return;
    }

    ListNode* current = head.get();
    while (current->next && current->next->data.startTime < newNode->data.startTime) {
        current = current->next.get();
    }
    newNode->next = std::move(current->next);
    current->next = std::move(newNode);
}

void LinkedList::append(Event e) {
    auto newNode = std::make_unique<ListNode>(std::move(e));
    if (!head) {
        head = std::move(newNode);
        return;
    }
    ListNode* temp = head.get();
    while (temp->next) {
        temp = temp->next.get();
    }
    temp->next = std::move(newNode);
}

bool LinkedList::removeByID(const std::string& id) {
    if (!head) return false;

    if (head->data.id == id) {
        head = std::move(head->next);
        return true;
    }

    ListNode* current = head.get();
    while (current->next) {
        if (current->next->data.id == id) {
            current->next = std::move(current->next->next);
            return true;
        }
        current = current->next.get();
    }
    return false;
}

Event* LinkedList::checkConflict(const std::string& start, const std::string& end) {
    int startMin = timeToMinutes(start);
    int endMin = timeToMinutes(end);
    ListNode* curr = head.get();
    while (curr) {
        int evtStart = timeToMinutes(curr->data.startTime);
        int evtEnd = timeToMinutes(curr->data.getEndTime());
        if (startMin < evtEnd && endMin > evtStart) {
            return &(curr->data);
        }
        curr = curr->next.get();
    }
    return nullptr;
}

int LinkedList::count() const {
    int c = 0;
    ListNode* curr = head.get();
    while (curr) {
        c++;
        curr = curr->next.get();
    }
    return c;
}

void LinkedList::clear() {
    head.reset();
}

std::string LinkedList::displayFormatted() const {
    std::string result;
    ListNode* curr = head.get();
    int count = 1;
    while (curr) {
        result += "\n  +--------------- EVENT #" + std::to_string(count) + " ----------------+\n";
        result += "  | Title    : " + curr->data.title;
        result += std::string(std::max(1, 33 - (int)curr->data.title.length()), ' ') + " |\n";
        result += "  | ID       : " + curr->data.id;
        result += std::string(std::max(1, 33 - (int)curr->data.id.length()), ' ') + " |\n";
        result += "  | Time     : " + curr->data.startTime + " - " + curr->data.getEndTime();
        result += std::string(std::max(1, 23 - (int)(curr->data.startTime.length() + curr->data.getEndTime().length())), ' ') + " |\n";
        result += "  | Priority : " + std::to_string(curr->data.priority);
        result += std::string(std::max(1, 32 - (int)std::to_string(curr->data.priority).length()), ' ') + " |\n";

        if (!curr->data.description.empty()) {
            result += "  | Descrip. : " + curr->data.description;
            result += std::string(std::max(1, 33 - (int)curr->data.description.length()), ' ') + " |\n";
        }

        result += "  +-----------------------------------------------+\n";
        curr = curr->next.get();
        count++;
    }
    return result;
}
