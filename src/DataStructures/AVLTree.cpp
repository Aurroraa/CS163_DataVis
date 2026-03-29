#include "../../include/DataStructures/AVLTree.h"
#include "../../include/Visualizer.h"
#include <algorithm>

AVLTree::~AVLTree() { clear(); }

void AVLTree::clear() {
    destroyTree(root);
    root = nullptr;
    nextNodeID = 0;
    nodeCount = 0;
}

void AVLTree::destroyTree(AVLNode *node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void AVLTree::calcPositions(AVLNode *node, float x, float y, float hSpacing) {
    if (!node) return;
    node->x = x;
    node->y = y;
    calcPositions(node->left, x - hSpacing, y + 80, hSpacing/2);
    calcPositions(node->right, x + hSpacing, y + 80, hSpacing/2);
}

void AVLTree::collectVisualNodes(AVLNode *node, std::vector<VisualNode> &visuals, int h1, int h2, Color c) {
    if (!node) return;

    VisualNode v;
    v.id = node->id;
    v.data = node->data;
    v.x = node->x;
    v.y = node->y;
    v.highlightIndex = getBalanceFactor(node);

    if (node->data == h1 || node->data == h2) v.color = c;
    else v.color = BLUE;

    v.nextNodeIndex = (node->left) ? node->left->id : -1;
    v.prevNodeIndex = (node->right) ? node->right->id : -1;
    visuals.push_back(v);

    collectVisualNodes(node->left, visuals, h1, h2, c);
    collectVisualNodes(node->right, visuals, h1, h2, c);
}

std::vector<VisualNode> AVLTree::captureState(int h1, int h2, Color c) {
    std::vector<VisualNode> visuals;
    calcPositions(root, GetScreenWidth()/2.0f, 100.0f, 300.0f);
    collectVisualNodes(root, visuals, h1, h2, c);
    return visuals;
}

int AVLTree::height(AVLNode *N) { return N ? N->height : 0; }
int AVLTree::getBalanceFactor(AVLNode *N) { return N ? height(N->left) - height(N->right) : 0; }

AVLNode *AVLTree::rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = std::max(height(y->left), height(y->right)) + 1;
    x->height = std::max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode *AVLTree::leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = std::max(height(x->left), height(x->right)) + 1;
    y->height = std::max(height(y->left), height(y->right)) + 1;

    return y;
}

void AVLTree::init(std::vector<int> values) {
    clear();
    Visualizer::Instance().RecordState("Cleared AVL Tree", 0, captureState(), {});
    int limit = std::min((int)values.size(), 50);
    for (int i = 0; i < limit; i++) insert(values[i]);
}

void AVLTree::insert(int value) {
    if (nodeCount >= 50) {
        Visualizer::Instance().RecordState("Error: Max 50 nodes allowed.", 0, captureState(), {});
        return;
    }

    std::vector<std::string> code = {"BST Insert", "Update Height", "Check Balance", "Rotate if needed"};

    if (!root) {
        root = new AVLNode(value, nextNodeID++);
        nodeCount++;
        Visualizer::Instance().RecordState("Created Root Node " + std::to_string(value), 0, captureState(value, -1, GREEN), code);
        return;
    }

    root = insertNode(root, value);
    Visualizer::Instance().RecordState("Insertion complete.", 0, captureState(-1, -1, BLUE), code);
}

AVLNode *AVLTree::insertNode(AVLNode *node, int value) {
    if (!node) {
        nodeCount++;
        return new AVLNode(value, nextNodeID++);
    }

    std::vector<std::string> code = {"BST Insert", "Update Height", "Check Balance", "Rotate if needed"};

    // 1. TOP-DOWN SEARCH: Highlight the current node we are comparing against!
    Visualizer::Instance().RecordState("Comparing " + std::to_string(value) + " with " + std::to_string(node->data), 0, captureState(node->data, -1, ORANGE), code);

    if (value < node->data) {
        bool wasNull = (node->left == nullptr);
        node->left = insertNode(node->left, value);
        if (wasNull) {
            // Highlight the newly inserted node!
            Visualizer::Instance().RecordState("Inserted " + std::to_string(value), 0, captureState(value, -1, GREEN), code);
        }
    }
    else if (value > node->data) {
        bool wasNull = (node->right == nullptr);
        node->right = insertNode(node->right, value);
        if (wasNull) {
            // Highlight the newly inserted node!
            Visualizer::Instance().RecordState("Inserted " + std::to_string(value), 0, captureState(value, -1, GREEN), code);
        }
    }
    else return node; // No duplicates allowed

    // 2. BOTTOM-UP BACKTRACK: Moving up to ancestors one by one as recursion unwinds!
    node->height = std::max(height(node->right), height(node->left)) + 1;
    int balanceFactor = getBalanceFactor(node);

    Visualizer::Instance().RecordState("Moving up to ancestor " + std::to_string(node->data) + ". Balance Factor is " + std::to_string(balanceFactor), 1, captureState(node->data, -1, PURPLE), code);

    // LL Case
    if (balanceFactor > 1 && value < node->left->data) {
        Visualizer::Instance().RecordState("Left-Left Imbalance. Right Rotating.", 3, captureState(node->data, -1, RED), code);
        return rightRotate(node);
    }
    // RR Case
    if (balanceFactor < -1 && value > node->right->data) {
        Visualizer::Instance().RecordState("Right-Right Imbalance. Left Rotating.", 3, captureState(node->data, -1, RED), code);
        return leftRotate(node);
    }
    // LR Case
    if (balanceFactor > 1 && value > node->left->data) {
        Visualizer::Instance().RecordState("Left-Right Imbalance. RL Rotating.", 3, captureState(node->data, -1, RED), code);
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // RL Case
    if (balanceFactor < -1 && value < node->right->data) {
        Visualizer::Instance().RecordState("Right-Left Imbalance. LR Rotating.", 3, captureState(node->data, -1, RED), code);
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Helper to find the Inorder Successor
AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

void AVLTree::deleteNode(int value) {
    if (nodeCount == 0) return;

    std::vector<std::string> code = {"Search", "If leaf: Delete", "If 1 child: Replace", "If 2 child: Swap successor & delete", "Update Heights", "Balance & Rotate"};
    root = deleteNodeHelper(root, value);
    Visualizer::Instance().RecordState("Deletion Complete", 0, captureState(-1, -1, BLUE), code);
}

AVLNode* AVLTree::deleteNodeHelper(AVLNode* node, int value) {
    std::vector<std::string> code = {"Search", "If leaf: Delete", "If 1 child: Replace", "If 2 child: Swap successor & delete", "Update Heights", "Balance & Rotate"};
    if (!node) return node;

    // TOP-DOWN SEARCH
    Visualizer::Instance().RecordState("Searching... at " + std::to_string(node->data), 0, captureState(node->data, -1, ORANGE), code);

    if (value < node->data) node->left = deleteNodeHelper(node->left, value);
    else if (value > node->data) node->right = deleteNodeHelper(node->right, value);
    else {
        Visualizer::Instance().RecordState("Node found!", 0, captureState(node->data, -1, RED), code);

        if (!node->left || !node->right) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node; node = nullptr;
            } else {
                *node = *temp;
            }
            delete temp;
            nodeCount--;
        } else {
            AVLNode* temp = minValueNode(node->right);
            Visualizer::Instance().RecordState("Successor found: " + std::to_string(temp->data), 3, captureState(node->data, temp->data, ORANGE), code);
            node->data = temp->data;
            node->right = deleteNodeHelper(node->right, temp->data);
        }
    }

    if (!node) return node;

    node->height = 1 + std::max(height(node->left), height(node->right));
    int balance = getBalanceFactor(node);

    // BOTTOM-UP BACKTRACK
    Visualizer::Instance().RecordState("Moving up to ancestor " + std::to_string(node->data) + ". Balance Factor is " + std::to_string(balance), 4, captureState(node->data, -1, PURPLE), code);

    if (balance > 1 && getBalanceFactor(node->left) >= 0) return rightRotate(node);
    if (balance > 1 && getBalanceFactor(node->left) < 0) { node->left = leftRotate(node->left); return rightRotate(node); }
    if (balance < -1 && getBalanceFactor(node->right) <= 0) return leftRotate(node);
    if (balance < -1 && getBalanceFactor(node->right) > 0) { node->right = rightRotate(node->right); return leftRotate(node); }

    return node;
}

void AVLTree::updateNode(int oldVal, int newVal) {
    deleteNode(oldVal);
    insert(newVal);
}

void AVLTree::searchNode(int value) {
    AVLNode* curr = root;
    while (curr) {
        Visualizer::Instance().RecordState("Comparing...", -1, captureState(curr->data, -1, ORANGE), {});
        if (curr->data == value) {
            Visualizer::Instance().RecordState("FOUND!", -1, captureState(curr->data, -1, GREEN), {});
            return;
        } else if (value < curr->data) curr = curr->left;
        else curr = curr->right;
    }
    Visualizer::Instance().RecordState("NOT FOUND.", -1, captureState(-1, -1, RED), {});
}