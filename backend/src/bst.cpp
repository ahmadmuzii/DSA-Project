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

// ---- AVL helpers ----

int BST::getHeight(const std::unique_ptr<BSTNode>& node) const {
    return node ? node->height : 0;
}

int BST::getHeight(BSTNode* node) const {
    return node ? node->height : 0;
}

int BST::getBalanceFactor(const std::unique_ptr<BSTNode>& node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

std::unique_ptr<BSTNode> BST::rotateRight(std::unique_ptr<BSTNode> y) {
    auto x = std::move(y->left);
    auto T2 = std::move(x->right);

    x->right = std::move(y);
    x->right->left = std::move(T2);

    x->right->height = std::max(getHeight(x->right->left), getHeight(x->right->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

std::unique_ptr<BSTNode> BST::rotateLeft(std::unique_ptr<BSTNode> x) {
    auto y = std::move(x->right);
    auto T2 = std::move(y->left);

    y->left = std::move(x);
    y->left->right = std::move(T2);

    y->left->height = std::max(getHeight(y->left->left), getHeight(y->left->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

// ---- insert with AVL balancing ----

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
        return node;
    }

    node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;

    int balance = getBalanceFactor(node);

    if (balance > 1 && convertToComparable(date) < convertToComparable(node->left->date))
        return rotateRight(std::move(node));

    if (balance < -1 && convertToComparable(date) > convertToComparable(node->right->date))
        return rotateLeft(std::move(node));

    if (balance > 1 && convertToComparable(date) > convertToComparable(node->left->date)) {
        node->left = rotateLeft(std::move(node->left));
        return rotateRight(std::move(node));
    }

    if (balance < -1 && convertToComparable(date) < convertToComparable(node->right->date)) {
        node->right = rotateRight(std::move(node->right));
        return rotateLeft(std::move(node));
    }

    return node;
}

// ---- search / forEach / display (unchanged) ----

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
    result += "   * " + node->date + " (" + std::to_string(countEvents(node->events.getHead())) + " events, h=" + std::to_string(node->height) + ")\n";
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
