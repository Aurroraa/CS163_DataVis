#pragma once
#include <vector>
#include "Global.h"

class MinHeap {
private:
    std::vector<int> data;

    std::vector<VisualNode> captureState(int h1 = -1, int h2 = -1, Color c = HIGHLIGHT_COLOR);

    void heapifyUp(int index);
    void heapifyDown(int index);
public:
    MinHeap() = default;
    ~MinHeap() = default;

    void init(std::vector<int> values);
    void insert(int value);
    void extractMin();

    int getSize() const {return data.size(); }
};