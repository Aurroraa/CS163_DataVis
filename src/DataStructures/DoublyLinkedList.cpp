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
std::vector<VisualNode> DoublyLinkedList::captureState(int highlightIndex, Color highlightColor) {
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

        if (index == highlightIndex) {
            v.color = highlightColor;
        }
        else v.color = BLUE;

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

void DoublyLinkedList::addAtIndex(int index, int value) {
    std::vector<std::string> code = {
        "if index == 0: addHead(val)",
        "if index == size: addTail(val)",
        "Traverse to index",
        "Node* newNode = new Node(val)",
        "newNode->next = curr",
        "newNode->prev = curr->prev",
        "curr->prev->next = newNode",
        "cur->prev = newNode"
    };

    if (index <= 0) {
        Visualizer::Instance().RecordState("Index is 0, routing to Add Head", 0, captureState(-1, GREEN), code);
        addHead(value);
        return;
    }

    int size = getCount();

    if (index >= size) {
        Visualizer::Instance().RecordState("Index >= size, routing to Add Tail", 1, captureState(-1, GREEN), code);
        addTail(value);
        return;
    }

    DLLNode* curr = head;
    for (int i = 0; i < index; i++) {
        Visualizer::Instance().RecordState("Traversing... at index " + std::to_string(i), 2, captureState(i, ORANGE), code);
        curr = curr->next;
    }

    Visualizer::Instance().RecordState("Found insertion point at index " + std::to_string(index), 2, captureState(index, ORANGE), code);

    DLLNode* newNode = new DLLNode{value, curr, curr->prev};
    if (curr -> prev) curr -> prev->next = newNode;
    curr -> prev = newNode;

    Visualizer::Instance().RecordState("Inserted " + std::to_string(value) + " at index " + std::to_string(index), 6, captureState(index, GREEN), code);
}

void DoublyLinkedList::deleteHead() {
    std::vector<std::string> code = {
        "if head == null: return",
        "Node* temp = head",
        "head = head->next",
        "if head != null: head->prev = null",
        "delete temp"
    };

    if (!head) {
        Visualizer::Instance().RecordState("List is already empty!", 0, captureState(), code);
        return;
    }

    Visualizer::Instance().RecordState("Targeting Head Node for deletion", 1, captureState(0, RED), code);

    DLLNode* temp = head;
    head = head->next;
    if (head) head->prev = nullptr;
    else tail = nullptr; // List became empty

    delete temp;

    Visualizer::Instance().RecordState("Deleted Head Node", 4, captureState(-1, GREEN), code);
}

void DoublyLinkedList::deleteTail() {
    std::vector<std::string> code = {
        "if tail == null: return",
        "Node* temp = tail",
        "tail = tail->prev",
        "if tail != null: tail->next = null",
        "delete temp"
    };

    if (!tail) {
        Visualizer::Instance().RecordState("List is already empty!", 0, captureState(), code);
        return;
    }

    // Count size to find the tail index for highlighting
    int tailIdx = 0;
    DLLNode* countTemp = head;
    while (countTemp && countTemp->next) { tailIdx++; countTemp = countTemp->next; }

    Visualizer::Instance().RecordState("Targeting Tail Node for deletion", 1, captureState(tailIdx, RED), code);

    DLLNode* temp = tail;
    tail = tail->prev;
    if (tail) tail->next = nullptr;
    else head = nullptr; // List became empty

    delete temp;

    Visualizer::Instance().RecordState("Deleted Tail Node", 4, captureState(-1, GREEN), code);
}

void DoublyLinkedList::deleteAtIndex(int index) {
    std::vector<std::string> code = {
        "if index == 0: deleteHead()",
        "Traverse to index",
        "if curr == tail: deleteTail()",
        "curr->prev->next = curr->next",
        "curr->next->prev = curr->prev",
        "delete curr"
    };

    if (index <= 0 || !head) {
        Visualizer::Instance().RecordState("Index is 0, routing to Delete Head", 0, captureState(), code);
        deleteHead();
        return;
    }

    DLLNode* curr = head;
    for (int i = 0; i < index; i++) {
        if (!curr->next) break; // Out of bounds prevention
        Visualizer::Instance().RecordState("Traversing... at index " + std::to_string(i), 1, captureState(i, ORANGE), code);
        curr = curr->next;
    }

    if (!curr->next) {
        Visualizer::Instance().RecordState("Reached end, routing to Delete Tail", 2, captureState(), code);
        deleteTail();
        return;
    }

    Visualizer::Instance().RecordState("Targeting node at index " + std::to_string(index) + " for deletion", 2, captureState(index, RED), code);

    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    delete curr;

    Visualizer::Instance().RecordState("Node deleted and pointers re-linked!", 5, captureState(-1, GREEN), code);
}

void DoublyLinkedList::updateAtIndex(int index, int newValue) {
    std::vector<std::string> code = {
        "Traverse to index",
        "if curr == null: return (Out of bounds)",
        "curr->data = newValue"
    };

    DLLNode* curr = head;
    for (int i = 0; i < index; i++) {
        if (!curr) break;
        Visualizer::Instance().RecordState("Traversing... at index " + std::to_string(i), 0, captureState(i, ORANGE), code);
        curr = curr->next;
    }

    if (!curr) {
        Visualizer::Instance().RecordState("Index out of bounds!", 1, captureState(), code);
        return;
    }

    Visualizer::Instance().RecordState("Found location! Changing " + std::to_string(curr->data) + " to " + std::to_string(newValue), 2, captureState(index, GREEN), code);
    curr->data = newValue;
    Visualizer::Instance().RecordState("Value Updated", 2, captureState(index, BLUE), code);
}

void DoublyLinkedList::searchNode(int value) {
    std::vector<std::string> code = {
        "curr = head",
        "while curr != null:",
        "  if curr->data == value: return FOUND",
        "  curr = curr->next",
        "return NOT FOUND"
    };

    DLLNode* curr = head;
    int index = 0;

    while (curr) {
        Visualizer::Instance().RecordState("Checking if " + std::to_string(curr->data) + " == " + std::to_string(value), 2, captureState(index, ORANGE), code);

        if (curr->data == value) {
            Visualizer::Instance().RecordState("Value " + std::to_string(value) + " FOUND at index " + std::to_string(index) + "!", 2, captureState(index, GREEN), code);
            return;
        }

        curr = curr->next;
        index++;
    }

    Visualizer::Instance().RecordState("Reached end of list. Value " + std::to_string(value) + " NOT FOUND.", 4, captureState(-1, RED), code);
}
