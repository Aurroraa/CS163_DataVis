#include "../../include/DataStructures/MinHeap.h"
#include "../../include/Visualizer.h"
#include <cmath>
using namespace std;

std::vector<VisualNode> MinHeap::captureState(int h1, int h2, Color highlightCol) {
    vector<VisualNode> visuals;
    int startY = 150;

    for (int i = 0; i < data.size(); i++) {
        VisualNode v;
        v.id = data[i].id;        // UNIQUE ID for animation
        v.highlightIndex = i;     // ARRAY INDEX for renderer
        v.data = data[i].value;   // ACTUAL VALUE

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
    nextNodeId = 0; // Reset IDs
    Visualizer::Instance().RecordState("Cleared Heap", 0, captureState(), {});

    for (int i = 0; i < values.size(); i++) insert(values[i]);

    Visualizer::Instance().RecordState("Heap Initialization Complete", 0, captureState(), {});
}

// ---------------------------------------------------------
// INSERT & HEAPIFY UP
// ---------------------------------------------------------
void MinHeap::insert(int value) {
    data.push_back({value, nextNodeId++});

    // The FULL code block for Insertion
    vector<string> code = {
        "heap.push_back(val)",                           // Line 0
        "cur = heap.size() - 1",                         // Line 1
        "while (cur > 0 && heap[par] > heap[cur]) {",    // Line 2
        "    swap(heap[cur], heap[par])",                // Line 3
        "    cur = par",                                 // Line 4
        "}"                                              // Line 5
    };

    // Pass Line 0
    Visualizer::Instance().RecordState("Inserted " + to_string(value) + " at end", 0, captureState(data.size() - 1, -1, GREEN), code);

    heapifyUp(data.size() - 1);
}

void MinHeap::heapifyUp(int index) {
    // Exact same block as insert so the UI doesn't flicker
    vector<string> code = {
        "heap.push_back(val)",                           // Line 0
        "cur = heap.size() - 1",                         // Line 1
        "while (cur > 0 && heap[par] > heap[cur]) {",    // Line 2
        "    swap(heap[cur], heap[par])",                // Line 3
        "    cur = par",                                 // Line 4
        "}"                                              // Line 5
    };

    while (index > 0) {
        int par = (index - 1) / 2;

        // Pass Line 2 (While Loop)
        Visualizer::Instance().RecordState("Comparing " + to_string(data[index].value) + " and " + to_string(data[par].value), 2, captureState(index, par, ORANGE), code );

        if (data[index].value < data[par].value) {
            swap(data[index], data[par]);
            // Pass Line 3 (Swap)
            Visualizer::Instance().RecordState("Swapped!", 3, captureState(index, par, GREEN), code);
            index = par;
        } else {
            // Pass Line 5 (End)
            Visualizer::Instance().RecordState("Heap property satisfied.", 5, captureState(), code);
            break;
        }
    }
}

// ---------------------------------------------------------
// EXTRACT MIN & HEAPIFY DOWN
// ---------------------------------------------------------
void MinHeap::extractMin() {
    if (data.empty()) return;

    vector<string> code = {
        "if (heap.empty()) return",                      // Line 0
        "swap(heap[0], heap[last])",                     // Line 1
        "heap.pop_back()",                               // Line 2
        "heapifyDown(0)"                                 // Line 3
    };

    if (data.size() == 1) {
        Visualizer::Instance().RecordState("Extracting root", 2, captureState(0, -1, RED), code); // Line 2
        data.pop_back();
        Visualizer::Instance().RecordState("Heap is empty", 3, captureState(), code);
        return;
    }

    int lastIdx = data.size() - 1;
    Visualizer::Instance().RecordState("Swapping Root with Last Element", 1, captureState(0, lastIdx, ORANGE), code); // Line 1
    swap(data[0], data[lastIdx]);

    Visualizer::Instance().RecordState("Root and Last Swapped", 2, captureState(0, lastIdx, GREEN), code); // Line 2
    data.pop_back();

    Visualizer::Instance().RecordState("Extracted minimum value", 3, captureState(), code); // Line 3
    heapifyDown(0);
}

void MinHeap::heapifyDown(int index) {
    vector<string> code = {
        "smallest = cur",                                // Line 0
        "if (left < size && heap[left] < heap[smallest])", // Line 1
        "    smallest = left",                           // Line 2
        "if (right < size && heap[right] < heap[smallest])",// Line 3
        "    smallest = right",                          // Line 4
        "if (smallest != cur) {",                        // Line 5
        "    swap(heap[cur], heap[smallest])",           // Line 6
        "    heapifyDown(smallest)",                     // Line 7
        "}"                                              // Line 8
    };

    int size = data.size();

    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        Visualizer::Instance().RecordState("Finding smallest child...", 1, captureState(index, -1, ORANGE), code); // Line 1

        if (left < size && data[left].value < data[smallest].value) smallest = left;
        if (right < size && data[right].value < data[smallest].value) smallest = right;

        if (smallest != index) {
            Visualizer::Instance().RecordState("Comparing parent with smallest child", 5, captureState(index, smallest, ORANGE), code); // Line 5
            swap(data[index], data[smallest]);
            Visualizer::Instance().RecordState("Swapped!", 6, captureState(index, smallest, GREEN), code); // Line 6
            index = smallest;
        } else {
            Visualizer::Instance().RecordState("Heap property satisfied.", 8, captureState(), code); // Line 8
            break;
        }
    }
}

// ---------------------------------------------------------
// DELETE AT INDEX
// ---------------------------------------------------------
void MinHeap::deleteNode(int index) {
    if (index < 0 || index >= data.size()) {
        Visualizer::Instance().RecordState("Index out of bounds!", 0, captureState(), {}); return;
    }

    vector<string> code = {
        "swap(heap[index], heap[last])",                 // Line 0
        "heap.pop_back()",                               // Line 1
        "if (heap[index] < heap[parent])",               // Line 2
        "    heapifyUp(index)",                          // Line 3
        "else",                                          // Line 4
        "    heapifyDown(index)"                         // Line 5
    };

    if (index == data.size() - 1) {
        Visualizer::Instance().RecordState("Deleting last element", 1, captureState(index, -1, RED), code); // Line 1
        data.pop_back();
        Visualizer::Instance().RecordState("Deleted", 1, captureState(), code);
        return;
    }

    int lastIdx = data.size() - 1;
    Visualizer::Instance().RecordState("Swapping with last node", 0, captureState(index, lastIdx, ORANGE), code); // Line 0
    swap(data[index], data[lastIdx]);

    Visualizer::Instance().RecordState("Removing last node...", 1, captureState(index, lastIdx, RED), code); // Line 1
    data.pop_back();

    Visualizer::Instance().RecordState("Checking where to Heapify...", 2, captureState(index, -1, BLUE), code); // Line 2

    int parent = (index - 1) / 2;
    if (index > 0 && data[index].value < data[parent].value) heapifyUp(index);
    else heapifyDown(index);
}

// ---------------------------------------------------------
// UPDATE AT INDEX
// ---------------------------------------------------------
void MinHeap::updateNode(int index, int newValue) {
    if (index < 0 || index >= data.size()) {
        Visualizer::Instance().RecordState("Index out of bounds!", 0, captureState(), {}); return;
    }
    vector<string> code = {
        "heap[index] = newValue",                        // Line 0
        "if (heap[index] < heap[parent])",               // Line 1
        "    heapifyUp(index)",                          // Line 2
        "else",                                          // Line 3
        "    heapifyDown(index)"                         // Line 4
    };

    int oldVal = data[index].value;
    data[index].value = newValue;
    Visualizer::Instance().RecordState("Value changed from " + to_string(oldVal) + " to " + to_string(newValue), 0, captureState(index, -1, GREEN), code); // Line 0

    Visualizer::Instance().RecordState("Checking where to Heapify...", 1, captureState(index, -1, BLUE), code); // Line 1

    int parent = (index - 1) / 2;
    if (index > 0 && data[index].value < data[parent].value) heapifyUp(index);
    else heapifyDown(index);
}

// ---------------------------------------------------------
// SEARCHING
// ---------------------------------------------------------
void MinHeap::searchNode(int value) {
    vector<string> code = {
        "for (int i = 0; i < heap.size(); i++) {",       // Line 0
        "    if (heap[i] == val)",                       // Line 1
        "        return FOUND",                          // Line 2
        "}",                                             // Line 3
        "return NOT_FOUND"                               // Line 4
    };

    for (int i = 0; i < data.size(); i++) {
        Visualizer::Instance().RecordState("Checking index " + to_string(i), 0, captureState(i, -1, ORANGE), code); // Line 0

        if (data[i].value == value) {
            Visualizer::Instance().RecordState("Found " + to_string(value) + " at index " + to_string(i) + "!", 2, captureState(i, -1, GREEN), code); // Line 2
            return;
        }
    }
    Visualizer::Instance().RecordState("Value not found in Heap.", 4, captureState(), code); // Line 4
}

void MinHeap::searchPosition(int index) {
    if (index < 0 || index >= data.size()) {
        Visualizer::Instance().RecordState("Index out of bounds", 0, captureState(), {}); return;
    }
    vector<string> code = { "return heap[index]" };
    Visualizer::Instance().RecordState("Node at position " + to_string(index) + " is " + to_string(data[index].value), 0, captureState(index, -1, GREEN), code);
}