#include "../../include/DataStructures/DoublyLinkedList.h"
#include "../../include/Visualizer.h"
#include <string>
#include <vector>

// ---------------------------------------------------------
// CONSTRUCTOR & DESTRUCTOR (MISSING PART)
// ---------------------------------------------------------
DoublyLinkedList::DoublyLinkedList() {
    head = nullptr;
    tail = nullptr;
}

DoublyLinkedList::~DoublyLinkedList() {
    DLLNode* current = head;
    while (current != nullptr) {
        DLLNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
}

// ---------------------------------------------------------
// HELPER: Count Nodes
// ---------------------------------------------------------
int DoublyLinkedList::getCount() {
    int count = 0;
    DLLNode* curr = head;
    while (curr) { count++; curr = curr->next; }
    return count;
}

// ---------------------------------------------------------
// 1. CAPTURE STATE (Single Line Layout)
// ---------------------------------------------------------
std::vector<VisualNode> DoublyLinkedList::captureState() {
    std::vector<VisualNode> visuals;
    DLLNode* current = head;
    int index = 0;
    int totalNodes = getCount();

    // 1. CONFIGURATION
    int spacing = 100;          // Standard spacing
    int nodeRadius = 30;

    // Safety: If screen is small, shrink spacing
    if (GetScreenWidth() < (totalNodes * spacing) + 100) {
        spacing = (GetScreenWidth() - 100) / (totalNodes > 0 ? totalNodes : 1);
        if (spacing < 65) spacing = 65; // Don't let them overlap too much
    }

    // 2. CENTER THE LIST
    // Calculate total width of the list
    int totalWidth = (totalNodes - 1) * spacing;
    // Find the starting X that centers this width on screen
    int startX = (GetScreenWidth() - totalWidth) / 2;
    int fixedY = GetScreenHeight() / 2 - 50; // Center vertically

    while (current != nullptr) {
        VisualNode v;
        v.id = index;
        v.data = current->data;
        v.color = BLUE;

        // 3. POSITIONING
        v.x = startX + (index * spacing);
        v.y = fixedY;

        v.nextNodeIndex = (current->next != nullptr) ? index + 1 : -1;
        v.prevNodeIndex = (current->prev != nullptr) ? index - 1 : -1;

        visuals.push_back(v);
        current = current->next;
        index++;
    }
    return visuals;
}

// ---------------------------------------------------------
// 2. INIT (With Limit Check)
// ---------------------------------------------------------
void DoublyLinkedList::init(std::vector<int> values) {
    // Clear old list
    DLLNode* current = head;
    while (current != nullptr) {
        DLLNode* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;

    // Limit input to 15
    if (values.size() > 15) {
        values.resize(15);
    }

    for (int val : values) {
        DLLNode* newNode = new DLLNode{val, nullptr, nullptr};
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    Visualizer::Instance().RecordState("Initialized List (Max 15)", 0, captureState(), {});
}

// ---------------------------------------------------------
// 3. ADD HEAD
// ---------------------------------------------------------
void DoublyLinkedList::addHead(int value) {
    if (getCount() >= 15) {
        Visualizer::Instance().RecordState("Error: Max 15 nodes allowed!", 0, captureState(), {});
        return;
    }

    std::vector<std::string> code = {
        "Node* newNode = new Node(val);",
        "if (!head) head = newNode;",
        "else { newNode->next = head; head->prev = newNode; }",
        "head = newNode;"
    };

    DLLNode* newNode = new DLLNode{value, nullptr, nullptr};
    Visualizer::Instance().RecordState("Creating Node " + std::to_string(value), 0, captureState(), code);

    if (head == nullptr) {
        head = tail = newNode;
        Visualizer::Instance().RecordState("List Empty: Head = New Node", 1, captureState(), code);
        return;
    }

    newNode->next = head;
    head->prev = newNode;
    Visualizer::Instance().RecordState("Linking New Node -> Old Head", 2, captureState(), code);

    head = newNode;
    Visualizer::Instance().RecordState("Updating Head Pointer", 3, captureState(), code);
}

// ---------------------------------------------------------
// 4. ADD TAIL
// ---------------------------------------------------------
void DoublyLinkedList::addTail(int value) {
    if (getCount() >= 15) {
        Visualizer::Instance().RecordState("Error: Max 15 nodes allowed!", 0, captureState(), {});
        return;
    }

    std::vector<std::string> code = {
        "Node* newNode = new Node(val);",
        "if (!head) head = newNode;",
        "else { tail->next = newNode; newNode->prev = tail; }",
        "tail = newNode;"
    };

    DLLNode* newNode = new DLLNode{value, nullptr, nullptr};
    Visualizer::Instance().RecordState("Creating Node " + std::to_string(value), 0, captureState(), code);

    if (head == nullptr) {
        head = tail = newNode;
        Visualizer::Instance().RecordState("List Empty: Head = New Node", 1, captureState(), code);
        return;
    }

    tail->next = newNode;
    newNode->prev = tail;
    Visualizer::Instance().RecordState("Linking Old Tail -> New Node", 2, captureState(), code);

    tail = newNode;
    Visualizer::Instance().RecordState("Updating Tail Pointer", 3, captureState(), code);
}

// ---------------------------------------------------------
// 5. DELETE NODE
// ---------------------------------------------------------
void DoublyLinkedList::deleteNode(int value) {
    std::vector<std::string> code = {
        "Node* curr = head;",
        "while (curr && curr->data != val) curr = curr->next;",
        "if (curr) { unlink(curr); delete curr; }"
    };

    if (!head) return;

    DLLNode* current = head;
    int index = 0;

    // Search
    while (current != nullptr && current->data != value) {
        std::vector<VisualNode> frame = captureState();
        if (index < frame.size()) frame[index].color = ORANGE;
        Visualizer::Instance().RecordState("Searching for " + std::to_string(value), 1, frame, code);

        current = current->next;
        index++;
    }

    if (!current) {
        Visualizer::Instance().RecordState("Value not found.", 1, captureState(), code);
        return;
    }

    // Found
    std::vector<VisualNode> frame = captureState();
    if (index < frame.size()) frame[index].color = RED;
    Visualizer::Instance().RecordState("Found target node", 2, frame, code);

    // Unlink
    if (current == head) {
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;
    } else if (current == tail) {
        tail = tail->prev;
        if (tail) tail->next = nullptr;
    } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }

    delete current;
    Visualizer::Instance().RecordState("Node Deleted", 2, captureState(), code);
}