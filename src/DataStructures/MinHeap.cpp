#include "DataStructures/MinHeap.h"
#include "Visualizer.h"
#include <cmath>
using namespace std;

std::vector<VisualNode> MinHeap::captureState(int h1, int h2, Color highlightCol) {
    vector<VisualNode> visuals;

    int startY = 150;

    for (int i = 0; i < data.size(); i++) {
        VisualNode v;
        v.id = i;
        v.data = data[i];

        if (i == h1 || i == h2) {
            v.color = highlightCol;
        }
        else v.color = BLUE;

        int depth = (i == 0) ? 0 : floor(log2(i + 1));
        int posInLevel = (i + 1) - pow(2, depth);
        int horizontalSpacing = GetScreenWidth() / (pow(2, depth) + 1);

        v.x = horizontalSpacing * (posInLevel + 1);
        v.y = startY + depth * 90;

        int leftChild = i * 2 + 1;
        int rightChild = i * 2 + 2;

        v.nextNodeIndex = (leftChild < data.size() ? leftChild : -1);
        v.prevNodeIndex = (rightChild < data.size() ? (rightChild) : -1);

        visuals.push_back(v);
    }
    return visuals;
}

void MinHeap::init(std::vector<int> values) {
    data.clear();
    Visualizer::Instance().RecordState("Cleared Heap", 0, captureState(), {});

    for (int i = 0; i < values.size(); i++) {
        insert(values[i]);
    }
    Visualizer::Instance().RecordState("Heap Initialization Complete", 0, captureState(), {});
}


void MinHeap::insert(int value) {
    data.push_back(value);

    vector<string> code = {"heap.push_back(val)", "heapifyUp(size - 1)"};

    Visualizer::Instance().RecordState("Inserted " + to_string(value) + " at end", 0, captureState(data.size() - 1, -1, GREEN), code);

    heapifyUp(data.size() - 1);
}

void MinHeap::heapifyUp(int index) {
    vector<string> code = {"while(cur > 0 && par > cur) swap();"};

    while (index > 0) {
        int par = (index - 1) / 2;
        Visualizer::Instance().RecordState("Comparing " + to_string(data[index]) + " and " + to_string(data[par]), 0, captureState(index, par, ORANGE), code );

        if (data[index] < data[par]) {
            swap(data[index], data[par]);

            Visualizer::Instance().RecordState("Swapped!", 0, captureState(index, par, GREEN), code);

            index = par;
        }
        else {
            Visualizer::Instance().RecordState("Heap property satisfied.", 0, captureState(), code);
            break;
        }
    }
}

void MinHeap::extractMin() {
    if (data.empty()) return;

    vector<string> code = {
        "swap(heap[0], heap[last])",
        "heap.pop_back()",
        "heapifyDown(0)"
    };

    if (data.size() == 1) {
        Visualizer::Instance().RecordState("Extracting root", 0, captureState(0, -1, RED), code);
        data.pop_back();
        Visualizer::Instance().RecordState("Heap is empty", 1, captureState(), code);
        return;
    }

    int lastIdx = data.size() - 1;

    Visualizer::Instance().RecordState("Swapping Root with Last Element", 0, captureState(0, lastIdx, ORANGE), code);

    swap(data[0], data[lastIdx]);

    Visualizer::Instance().RecordState("Root and Last Swapped", 0, captureState(0, lastIdx, GREEN), code);
    data.pop_back();

    Visualizer::Instance().RecordState("Extracted minimum value", 1, captureState(), code);

    heapifyDown(0);

}

void MinHeap::heapifyDown(int index) {
    vector<string> code = {
        "Find min_child, if min_child < cur, swap();"
    };

    int size = data.size();

    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        if (left < size && data[left] < data[smallest]) smallest = left;
        if (right < size && data[right] < data[smallest]) smallest = right;

        if (smallest != index) {
            Visualizer::Instance().RecordState("Comparing parent with smallest child", 0, captureState(index, smallest, ORANGE), code);

            swap(data[index], data[smallest]);

            Visualizer::Instance().RecordState("Swapped!", 0, captureState(index, smallest, GREEN), code);
            index = smallest;
        }
        else {
            Visualizer::Instance().RecordState("Heap property satisfied.", 0, captureState(), code);
            break;
        }

    }
}




