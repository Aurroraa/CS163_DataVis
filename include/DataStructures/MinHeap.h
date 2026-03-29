#pragma once
#include <vector>
#include <string>
#include "../Global.h"

struct HeapElement {
    int value;
    int id;
};

class MinHeap {
private:
    std::vector<HeapElement> data;
    int nextNodeId = 0; // Tracks unique IDs

public:
    MinHeap() = default;
    void init(std::vector<int> values);
    void insert(int value);
    void extractMin();
    void deleteNode(int index);
    void updateNode(int index, int newValue);
    void searchNode(int value);
    void searchPosition(int index);

    std::vector<VisualNode> captureState(int h1 = -1, int h2 = -1, Color highlightCol = BLUE);

private:
    void heapifyUp(int index);
    void heapifyDown(int index);
};