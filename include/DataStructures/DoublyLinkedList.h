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



    // NEW Helper
    int getCount();

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void init(std::vector<int> values);
    void addHead(int value);
    void addTail(int value);
    void addAtIndex(int index, int value);
    void deleteHead();
    void deleteTail();
    void deleteAtIndex(int index);
    void updateAtIndex(int index, int value);
    void searchNode(int value);

    // Helper to position nodes for drawing
    std::vector<VisualNode> captureState(int highlightIndex = -1, Color highlightColor = BLUE);
};