#ifndef BST_H
#define BST_H

#include "linked_list.h"
#include <functional>
#include <memory>
#include <string>

struct BSTNode {
    std::string date;
    LinkedList events;
    std::unique_ptr<BSTNode> left;
    std::unique_ptr<BSTNode> right;

    BSTNode(const std::string& d) : date(d) {}
};

class BST {
    std::unique_ptr<BSTNode> root;

public:
    BST() = default;
    BST(const BST& other);
    BST& operator=(const BST& other);
    BST(BST&&) = default;
    BST& operator=(BST&&) = default;

    void insert(const std::string& date, Event e);
    BSTNode* search(const std::string& date);
    void forEach(std::function<void(const Event&)> callback) const;
    bool isEmpty() const { return root == nullptr; }
    std::string displayDates() const;
    BSTNode* getRoot() const { return root.get(); }

private:
    std::unique_ptr<BSTNode> insertRec(std::unique_ptr<BSTNode> node, const std::string& date, Event e);
    BSTNode* searchRec(BSTNode* node, const std::string& date);
    void forEachRec(BSTNode* node, std::function<void(const Event&)> callback) const;
    void displayDatesRec(BSTNode* node, std::string& result) const;
    int countEvents(ListNode* head) const;
    void copyTree(BSTNode* node);
};

#endif
