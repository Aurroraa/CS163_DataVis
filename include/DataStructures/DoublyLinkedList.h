#pragma once
#include "VisualNode.h"
#include "../Visualizer.h"
#include "Global.h"
#include <vector>
#include <string>
using namespace std;

struct DLLNode {
    int data;
    DLLNode* next;
    DLLNode* prev;
};

class DoublyLinkedList {
private:
    DLLNode* head;
    DLLNode* tail;
    vector<VisualNode> captureState();
public:
    DoublyLinkedList();
    ~DoublyLinkedList();
    void init(vector<int> values);
    void addHead(int value);
    void addTail(int value);
    void deleteNode(int value);
    void searchNode(int value);

    void setRecord(Visualizer* Vis);
};