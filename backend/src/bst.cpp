#include "bst.h"
#include "ds_utility.h"
#include <iostream>

BST::BST(const BST& other) {
    copyTree(other.root.get());
}

BST& BST::operator=(const BST& other) {
    if (this != &other) {
        root.reset();
        copyTree(other.root.get());
    }
    return *this;
}

void BST::copyTree(BSTNode* node) {
    if (!node) return;

    ListNode* curr = node->events.getHead();
    while (curr) {
        insert(node->date, curr->data);
        curr = curr->next.get();
    }

    copyTree(node->left.get());
    copyTree(node->right.get());
}

void BST::insert(const std::string& date, Event e) {
    root = insertRec(std::move(root), date, std::move(e));
}

std::unique_ptr<BSTNode> BST::insertRec(std::unique_ptr<BSTNode> node, const std::string& date, Event e) {
    if (!node) {
        auto newNode = std::make_unique<BSTNode>(date);
        newNode->events.insertSorted(std::move(e));
        return newNode;
    }

    std::string date1 = convertToComparable(date);
    std::string date2 = convertToComparable(node->date);

    if (date1 < date2) {
        node->left = insertRec(std::move(node->left), date, std::move(e));
    } else if (date1 > date2) {
        node->right = insertRec(std::move(node->right), date, std::move(e));
    } else {
        node->events.insertSorted(std::move(e));
    }
    return node;
}

BSTNode* BST::search(const std::string& date) {
    return searchRec(root.get(), date);
}

BSTNode* BST::searchRec(BSTNode* node, const std::string& date) {
    if (!node) return nullptr;

    std::string date1 = convertToComparable(date);
    std::string date2 = convertToComparable(node->date);

    if (date1 == date2) return node;
    if (date1 < date2) return searchRec(node->left.get(), date);
    return searchRec(node->right.get(), date);
}

void BST::forEach(std::function<void(const Event&)> callback) const {
    forEachRec(root.get(), callback);
}

void BST::forEachRec(BSTNode* node, std::function<void(const Event&)> callback) const {
    if (!node) return;

    forEachRec(node->left.get(), callback);

    ListNode* curr = node->events.getHead();
    while (curr) {
        callback(curr->data);
        curr = curr->next.get();
    }

    forEachRec(node->right.get(), callback);
}

std::string BST::displayDates() const {
    std::string result;
    if (root) {
        result += "   Root Node Date: " + root->date + "\n";
        result += "   Tree Structure (In-order):\n";
        displayDatesRec(root.get(), result);
    } else {
        result += "   BST is Empty\n";
    }
    return result;
}

void BST::displayDatesRec(BSTNode* node, std::string& result) const {
    if (!node) return;

    displayDatesRec(node->left.get(), result);
    result += "   * " + node->date + " (" + std::to_string(countEvents(node->events.getHead())) + " events)\n";
    displayDatesRec(node->right.get(), result);
}

int BST::countEvents(ListNode* head) const {
    int c = 0;
    while (head) {
        c++;
        head = head->next.get();
    }
    return c;
}
