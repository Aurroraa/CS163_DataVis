#include "DataStructures/DoublyLinkedList.h"
#include "../../include/DataStructures/Visualizer.h"

using namespace std;

extern Visualizer* globalApp;

DoublyLinkedList::DoublyLinkedList(){
    head = nullptr;
    tail = nullptr;
}

vector<VisualNode> DoublyLinkedList::captureState() {
    std::vector<VisualNode> visuals;
    DLLNode* current = head;
    int index = 0;

    while (current != nullptr) {
        VisualNode v;
        v.id = index;
        v.data = current->data;

        // Calculate Position: Line them up horizontally
        v.x = 100 + index * 120;
        v.y = 300;

        v.color = PRIMARY_COLOR; // Default Blue

        // Map pointers to indices for the visualizer
        v.nextNodeIndex = (current->next != nullptr) ? index + 1 : -1;
        v.prevNodeIndex = (current->prev != nullptr) ? index - 1 : -1;

        visuals.push_back(v);
        current = current->next;
        index++;
    }
    return visuals;
}


void DoublyLinkedList::init(vector<int> values) {

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

    if (globalApp) {
        // captureState() converts our pointers to draw-able data
        globalApp->recordState("Initialized List from file", 0, captureState());
    }
}

void DoublyLinkedList::addHead(int value) {

    if (globalApp) {
        globalApp->setCodeLines({
            "Algorithm: Add to Head",
            "1. Create a new node with value " + std::to_string(value),
            "2. If list is empty, set Head = New Node",
            "3. Else: Link New Node -> Old Head",
            "4.       Link Old Head <- New Node",
            "5. Update Head pointer to New Node"                               // Line 8
        });
    }

    DLLNode* newNode = new DLLNode{value, nullptr, nullptr};
    if (globalApp) {
        globalApp->recordState("Creating new node...", 1, captureState());
    }

    if (head == nullptr) {
        head = tail = newNode;
        if (globalApp) {
            globalApp->recordState("List was empty, set Head = NewNode", 2, captureState());
        }
        return;
    }
    if (globalApp) globalApp->recordState("Linking Next...", 4, captureState());
    newNode->next = head;
    if (globalApp) globalApp->recordState("Linking Prev...", 5, captureState());
    head->prev = newNode;
    if (globalApp) globalApp->recordState("Updating Head...", 6, captureState());
    head = newNode;

}

void DoublyLinkedList::addTail(int value) {
    DLLNode* newNode = new DLLNode{value, nullptr, nullptr};
    if (head == nullptr) {
        head = tail = newNode;
        if (globalApp) {
            globalApp->recordState("Initialized Tail", 1, captureState());
        }
        return;
    }
    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;

    if (globalApp) {
        globalApp->recordState("Added node to Tail", 2, captureState());
    }
}

void DoublyLinkedList::deleteNode(int value) {
    // 1. SETUP CODE BOX (Fixing the missing code text)
    if (globalApp) {
        globalApp->setCodeLines({
            "void deleteNode(int value) {",       // 0
            "   Node* current = head;",           // 1
            "   while(curr && curr->data != v)",  // 2
            "   // Found: Re-wire pointers",      // 3
            "   curr->prev->next = curr->next;",  // 4
            "   curr->next->prev = curr->prev;",  // 5
            "   delete current;",                 // 6
            "}"
        });
    }

    if (head == nullptr) return;

    DLLNode* current = head;
    int index = 0;

    // --- SEARCH ANIMATION ---
    while (current != nullptr && current->data != value) {
        if (globalApp) {
            vector<VisualNode> frame = captureState();
            frame[index].color = HIGHLIGHT_COLOR; // Orange
            globalApp->recordState("Searching for " + to_string(value) + "...", 2, frame);
        }
        current = current->next;
        index++;
    }

    if (current == nullptr) {
        if (globalApp) globalApp->recordState("Value not found.", 2, captureState());
        return;
    }

    // --- MARK TARGET ---
    if (globalApp) {
        vector<VisualNode> frame = captureState();
        frame[index].color = RED; // Show Red before deleting
        globalApp->recordState("Found target node!", 3, frame);
    }

    // --- LOGIC: RE-WIRE ---
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

    DLLNode* toDelete = current;

    // --- FIX: MANUAL GHOST NODE ---
    // At this point, 'captureState()' will MISS the deleted node because pointers bypass it.
    // We must manually add it back so the user sees the "Bypass" step.
    if (globalApp) {
        vector<VisualNode> frame = captureState(); // Captures the NEW list (without node)

        // Create the "Ghost" node
        VisualNode ghost;
        ghost.id = -1;
        ghost.data = toDelete->data;
        ghost.x = 100 + index * 120; // Use the old index position
        ghost.y = 300; // Same Y
        ghost.color = RED;
        ghost.nextNodeIndex = -1; // Disconnected
        ghost.prevNodeIndex = -1;

        // Add it to the frame
        frame.push_back(ghost);

        globalApp->recordState("Re-wired pointers to bypass node", 4, frame);
    }

    // --- CLEANUP ---
    delete toDelete;

    if (globalApp) {
        globalApp->recordState("Node deleted and memory freed", 6, captureState());
    }
}

void DoublyLinkedList::searchNode(int value) {

}

DoublyLinkedList::~DoublyLinkedList() {
    // We need to delete all nodes to prevent memory leaks
    DLLNode* current = head;
    while (current != nullptr) {
        DLLNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
}