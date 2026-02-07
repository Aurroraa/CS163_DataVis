#include "../../include/DataStructures/DoublyLinkedList.h"
#include "../../include/Visualizer.h" // Access the Singleton
#include <string>
#include <vector>

using namespace std;

// REMOVED: extern Visualizer* globalApp;  <-- This was causing the error!

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

// Helper to capture the current state of the list for the animation
vector<VisualNode> DoublyLinkedList::captureState() {
    vector<VisualNode> visuals;
    DLLNode* current = head;
    int index = 0;

    while (current != nullptr) {
        VisualNode v;
        v.id = index;
        v.data = current->data;
        // Simple positioning: Start at x=100, space them out by 120px
        v.x = 100 + index * 120;
        v.y = 300;
        v.color = BLUE; // Default color

        // Link logic for the renderer
        v.nextNodeIndex = (current->next != nullptr) ? index + 1 : -1;
        v.prevNodeIndex = (current->prev != nullptr) ? index - 1 : -1;

        visuals.push_back(v);
        current = current->next;
        index++;
    }
    return visuals;
}

void DoublyLinkedList::init(vector<int> values) {
    // 1. Clear old memory if any
    DLLNode* current = head;
    while (current != nullptr) {
        DLLNode* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;

    // 2. Build new list
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

    // 3. Record the initial state
    // We pass empty code lines {} because this isn't an algorithm step
    Visualizer::Instance().RecordState("Initialized List", 0, captureState(), {});
}

void DoublyLinkedList::addHead(int value) {
    // Define the pseudocode for this operation
    vector<string> code = {
        "Node* newNode = new Node(val);",
        "if (!head) head = newNode;",
        "else { newNode->next = head; head->prev = newNode; }",
        "head = newNode;"
    };

    DLLNode* newNode = new DLLNode{value, nullptr, nullptr};

    // Step 1: Create Node
    Visualizer::Instance().RecordState("Creating Node " + to_string(value), 0, captureState(), code);

    if (head == nullptr) {
        head = tail = newNode;
        // Step 2: Set Head
        Visualizer::Instance().RecordState("List Empty: Head = New Node", 1, captureState(), code);
        return;
    }

    // Step 3: Link
    newNode->next = head;
    head->prev = newNode;
    Visualizer::Instance().RecordState("Linking New Node -> Old Head", 2, captureState(), code);

    // Step 4: Update Head
    head = newNode;
    Visualizer::Instance().RecordState("Updating Head Pointer", 3, captureState(), code);
}

void DoublyLinkedList::addTail(int value) {
    vector<string> code = {
        "Node* newNode = new Node(val);",
        "if (!head) head = newNode;",
        "else { tail->next = newNode; newNode->prev = tail; }",
        "tail = newNode;"
    };

    DLLNode* newNode = new DLLNode{value, nullptr, nullptr};

    // Step 1: Create
    Visualizer::Instance().RecordState("Creating Node " + to_string(value), 0, captureState(), code);

    if (head == nullptr) {
        head = tail = newNode;
        Visualizer::Instance().RecordState("List Empty: Head = New Node", 1, captureState(), code);
        return;
    }

    // Step 2: Link
    tail->next = newNode;
    newNode->prev = tail;
    Visualizer::Instance().RecordState("Linking Old Tail -> New Node", 2, captureState(), code);

    // Step 3: Update Tail
    tail = newNode;
    Visualizer::Instance().RecordState("Updating Tail Pointer", 3, captureState(), code);
}

void DoublyLinkedList::deleteNode(int value) {
    vector<string> code = {
        "Node* curr = head;",
        "while (curr && curr->data != val) curr = curr->next;",
        "if (curr) { unlink(curr); delete curr; }"
    };

    if (!head) return;

    DLLNode* current = head;
    int index = 0;

    // Search Animation
    while (current != nullptr && current->data != value) {
        vector<VisualNode> frame = captureState();
        frame[index].color = ORANGE; // Highlight searching
        Visualizer::Instance().RecordState("Searching for " + to_string(value), 1, frame, code);

        current = current->next;
        index++;
    }

    if (!current) {
        Visualizer::Instance().RecordState("Value not found.", 1, captureState(), code);
        return;
    }

    // Found Animation
    vector<VisualNode> frame = captureState();
    frame[index].color = RED; // Mark for deletion
    Visualizer::Instance().RecordState("Found target node", 2, frame, code);

    // Unlink Logic
    if (current == head) {
        head = head->next;
        if (head) head->prev = nullptr;
    } else if (current == tail) {
        tail = tail->prev;
        if (tail) tail->next = nullptr;
    } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }

    // Show "Rewiring" (Ghost Node)
    // We manually add the red node back to the visual frame because it's detached
    vector<VisualNode> rewiredFrame = captureState();
    VisualNode ghost;
    ghost.id = -1;
    ghost.data = current->data;
    ghost.x = 100 + index * 120;
    ghost.y = 300;
    ghost.color = RED;
    ghost.nextNodeIndex = -1;
    rewiredFrame.push_back(ghost);

    Visualizer::Instance().RecordState("Re-wired pointers", 2, rewiredFrame, code);

    delete current;

    // Final State
    Visualizer::Instance().RecordState("Memory freed", 2, captureState(), code);
}