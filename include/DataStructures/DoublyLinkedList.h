#pragma once
#include <vector>
#include "../Global.h"

struct DLLNode {
    int data;
    DLLNode* next;
    DLLNode* prev;
};

class DoublyLinkedList {
private:
    DLLNode* head;
    DLLNode* tail;

    // Helper to position nodes for drawing
    std::vector<VisualNode> captureState();

    // NEW Helper
    int getCount();

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void init(std::vector<int> values);
    void addHead(int value);
    void addTail(int value);
    void deleteNode(int value);
};