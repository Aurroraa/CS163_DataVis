#include "../../include/DataStructures/MinHeap.h"
#include "../../include/Visualizer.h"
#include <cmath>
using namespace std;

std::vector<VisualNode> MinHeap::captureState(int h1, int h2, Color highlightCol) {
    vector<VisualNode> visuals;
    int startY = 150;

    for (int i = 0; i < data.size(); i++) {
        VisualNode v;
        v.id = data[i].id;
        v.highlightIndex = i;
        v.data = data[i].value;

        if (i == h1 || i == h2) v.color = highlightCol;
        else v.color = BLUE;

        int depth = (i == 0) ? 0 : floor(log2(i + 1));
        int posInLevel = (i + 1) - pow(2, depth);
        int horizontalSpacing = GetScreenWidth() / (pow(2, depth) + 1);

        v.x = horizontalSpacing * (posInLevel + 1);
        v.y = startY + depth * 90;

        int leftChild = i * 2 + 1;
        int rightChild = i * 2 + 2;

        v.nextNodeIndex = (leftChild < data.size() ? leftChild : -1);
        v.prevNodeIndex = (rightChild < data.size() ? rightChild : -1);

        visuals.push_back(v);
    }
    return visuals;
}

void MinHeap::init(std::vector<int> values) {
    data.clear();
    nextNodeId = 0;
    Visualizer::Instance().RecordState("Cleared Heap", 0, captureState(), {});

    for (int i = 0; i < values.size(); i++) insert(values[i]);

    Visualizer::Instance().RecordState("Heap Initialization Complete", 0, captureState(), {});
}


void MinHeap::insert(int value) {
    data.push_back({value, nextNodeId++});

    vector<string> code = {
        "heap.push_back(val)",
        "cur = heap.size() - 1",
        "while (cur > 0 && heap[par] > heap[cur]) {",
        "    swap(heap[cur], heap[par])",
        "    cur = par",
        "}"
    };

    Visualizer::Instance().RecordState("Inserted " + to_string(value) + " at end", 0, captureState(data.size() - 1, -1, GREEN), code);

    heapifyUp(data.size() - 1);
}

void MinHeap::heapifyUp(int index) {
    vector<string> code = {
        "heap.push_back(val)",
        "cur = heap.size() - 1",
        "while (cur > 0 && heap[par] > heap[cur]) {",
        "    swap(heap[cur], heap[par])",
        "    cur = par",
        "}"
    };

    while (index > 0) {
        int par = (index - 1) / 2;

        Visualizer::Instance().RecordState("Comparing " + to_string(data[index].value) + " and " + to_string(data[par].value), 2, captureState(index, par, ORANGE), code );

        if (data[index].value < data[par].value) {
            swap(data[index], data[par]);
            Visualizer::Instance().RecordState("Swapped!", 3, captureState(index, par, GREEN), code);
            index = par;
        } else {
            Visualizer::Instance().RecordState("Heap property satisfied.", 5, captureState(), code);
            break;
        }
    }
}


void MinHeap::extractMin() {
    if (data.empty()) return;

    vector<string> code = {
        "if (heap.empty()) return",
        "swap(heap[0], heap[last])",
        "heap.pop_back()",
        "heapifyDown(0)"
    };

    if (data.size() == 1) {
        Visualizer::Instance().RecordState("Extracting root", 2, captureState(0, -1, RED), code);
        data.pop_back();
        Visualizer::Instance().RecordState("Heap is empty", 3, captureState(), code);
        return;
    }

    int lastIdx = data.size() - 1;
    Visualizer::Instance().RecordState("Swapping Root with Last Element", 1, captureState(0, lastIdx, ORANGE), code);
    swap(data[0], data[lastIdx]);

    Visualizer::Instance().RecordState("Root and Last Swapped", 2, captureState(0, lastIdx, GREEN), code);
    data.pop_back();

    Visualizer::Instance().RecordState("Extracted minimum value", 3, captureState(), code);
    heapifyDown(0);
}

void MinHeap::heapifyDown(int index) {
    vector<string> code = {
        "smallest = cur",
        "if (left < size && heap[left] < heap[smallest])",
        "    smallest = left",
        "if (right < size && heap[right] < heap[smallest])",
        "    smallest = right",
        "if (smallest != cur) {",
        "    swap(heap[cur], heap[smallest])",
        "    heapifyDown(smallest)",
        "}"
    };

    int size = data.size();

    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        Visualizer::Instance().RecordState("Finding smallest child...", 1, captureState(index, -1, ORANGE), code);

        if (left < size && data[left].value < data[smallest].value) smallest = left;
        if (right < size && data[right].value < data[smallest].value) smallest = right;

        if (smallest != index) {
            Visualizer::Instance().RecordState("Comparing parent with smallest child", 5, captureState(index, smallest, ORANGE), code);
            swap(data[index], data[smallest]);
            Visualizer::Instance().RecordState("Swapped!", 6, captureState(index, smallest, GREEN), code);
            index = smallest;
        } else {
            Visualizer::Instance().RecordState("Heap property satisfied.", 8, captureState(), code);
            break;
        }
    }
}


void MinHeap::deleteNode(int index) {
    if (index < 0 || index >= data.size()) {
        Visualizer::Instance().RecordState("Index out of bounds!", 0, captureState(), {}); return;
    }

    vector<string> code = {
        "swap(heap[index], heap[last])",
        "heap.pop_back()",
        "if (heap[index] < heap[parent])",
        "    heapifyUp(index)",
        "else",
        "    heapifyDown(index)"
    };

    if (index == data.size() - 1) {
        Visualizer::Instance().RecordState("Deleting last element", 1, captureState(index, -1, RED), code);
        data.pop_back();
        Visualizer::Instance().RecordState("Deleted", 1, captureState(), code);
        return;
    }

    int lastIdx = data.size() - 1;
    Visualizer::Instance().RecordState("Swapping with last node", 0, captureState(index, lastIdx, ORANGE), code);
    swap(data[index], data[lastIdx]);

    Visualizer::Instance().RecordState("Removing last node...", 1, captureState(index, lastIdx, RED), code);
    data.pop_back();

    Visualizer::Instance().RecordState("Checking where to Heapify...", 2, captureState(index, -1, BLUE), code);

    int parent = (index - 1) / 2;
    if (index > 0 && data[index].value < data[parent].value) heapifyUp(index);
    else heapifyDown(index);
}

void MinHeap::updateNode(int index, int newValue) {
    if (index < 0 || index >= data.size()) {
        Visualizer::Instance().RecordState("Index out of bounds!", 0, captureState(), {}); return;
    }
    vector<string> code = {
        "heap[index] = newValue",
        "if (heap[index] < heap[parent])",
        "    heapifyUp(index)",
        "else",
        "    heapifyDown(index)"
    };

    int oldVal = data[index].value;
    data[index].value = newValue;
    Visualizer::Instance().RecordState("Value changed from " + to_string(oldVal) + " to " + to_string(newValue), 0, captureState(index, -1, GREEN), code);

    Visualizer::Instance().RecordState("Checking where to Heapify...", 1, captureState(index, -1, BLUE), code);

    int parent = (index - 1) / 2;
    if (index > 0 && data[index].value < data[parent].value) heapifyUp(index);
    else heapifyDown(index);
}


void MinHeap::searchNode(int value) {
    vector<string> code = {
        "for (int i = 0; i < heap.size(); i++) {",
        "    if (heap[i] == val)",
        "        return FOUND",
        "}",
        "return NOT_FOUND"
    };

    for (int i = 0; i < data.size(); i++) {
        Visualizer::Instance().RecordState("Checking index " + to_string(i), 0, captureState(i, -1, ORANGE), code);

        if (data[i].value == value) {
            Visualizer::Instance().RecordState("Found " + to_string(value) + " at index " + to_string(i) + "!", 2, captureState(i, -1, GREEN), code);
            return;
        }
    }
    Visualizer::Instance().RecordState("Value not found in Heap.", 4, captureState(), code);
}

void MinHeap::searchPosition(int index) {
    if (index < 0 || index >= data.size()) {
        Visualizer::Instance().RecordState("Index out of bounds", 0, captureState(), {}); return;
    }
    vector<string> code = { "return heap[index]" };
    Visualizer::Instance().RecordState("Node at position " + to_string(index) + " is " + to_string(data[index].value), 0, captureState(index, -1, GREEN), code);
}