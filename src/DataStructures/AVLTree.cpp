#include "DataStructures/AVLTree.h"
#include "Visualizer.h"
#include <algorithm>

AVLTree::~AVLTree() {clear(); }
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

    if (node -> data != h1 || node -> data != h2) v.color = c;
    else v.color = BLUE;

    v.nextNodeIndex = (node -> left) ? node -> left -> id : -1;
    v.prevNodeIndex = (node -> right) ? node -> right -> id : -1;
    visuals.push_back(v);

    collectVisualNodes(node -> left, visuals, h1, h2, c);
    collectVisualNodes(node->right, visuals, h1, h2, c);
}

std::vector<VisualNode> AVLTree::captureState(int h1, int h2, Color c) {
    std::vector<VisualNode> visuals;
    calcPositions(root, GetScreenWidth()/2.0f, 100.0f, 300.0f);
    collectVisualNodes(root, visuals, h1, h2, c);
    return visuals;
}

int AVLTree::height(AVLNode *N) {
    return N ? N->height : 0;
}

int AVLTree::getBalanceFactor(AVLNode *N) {
    return N ? height(N->left) - height(N->right) : 0;
}

AVLNode *AVLTree::rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode* T2 = x -> right;

    x -> right = y;
    y -> left = T2;

    y -> height = std::max(height(y -> left), height(y -> right)) + 1;
    x -> height = std::max(height(x -> right), height(x -> left)) + 1;

    return x;
}

AVLNode *AVLTree::leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode* T2 = y -> left;

    y -> left = x;
    x -> right = T2;

    x -> height = std::max(height(x -> left), height(x -> right)) + 1;
    y -> height = std::max(height(y -> left), height(y -> right)) + 1;
    return y;
}

void AVLTree::init(std::vector<int> values) {
    clear();
    Visualizer::Instance().RecordState("Cleared AVL Tree", 0, captureState(), {});
    int limit = std::min((int)values.size(), 50);
    for (int i = 0; i < limit; i++) {
        insert(values[i]);
    }
}

void AVLTree::insert(int value) {
    if (nodeCount > 50) {
        Visualizer::Instance().RecordState("Error: Max 50 nodes allowed.", 0, captureState(), {});
        return;
    }

    std::vector<std::string> code = {"BST Insert", "Update Height", "Check Balance", "Rotate if needed"};
    root = insertNode(root, value);
    Visualizer::Instance().RecordState("Inserted " + std::to_string(value), 0, captureState(value, -1, GREEN), code);
}

AVLNode *AVLTree::insertNode(AVLNode *node, int value) {
    if (!node) {
        nodeCount++;
        return new AVLNode(value, nextNodeID++);
    }
    std::vector<std::string> code = {"BST Insert", "Update Height", "Check Balance", "Rotate if needed"};
    Visualizer::Instance().RecordState("Comparing " + std::to_string(value) + " with " + std::to_string(node->data), 0, captureState(value, node->data, ORANGE), code);

    if (value < node->data) {
        node->left = insertNode(node->left, value);
    }
    else if (value > node->data) {
        node->right = insertNode(node->right, value);
    }
    else return node;

    node -> height = std::max(height(node->right), height(node->left)) + 1;
    int balanceFactor = getBalanceFactor(node);
    //LL
    if (balanceFactor > 1 && value < node -> left -> data) {
        Visualizer::Instance().RecordState("Left-Left Imbalance at " + std::to_string(node -> data) + ". Right Rotating.", 3, captureState(node -> data, -1, RED), code);
        return rightRotate(node);
    }
    //RR
    if (balanceFactor < -1 && value > node -> right -> data) {
        Visualizer::Instance().RecordState("Right-Right Imbalance at " + std::to_string(node -> data) + ". Left Rotating.", 3, captureState(node -> data, -1, RED), code);
        return leftRotate(node);
    }

    //LR
    if (balanceFactor > 1 && value > node -> left -> data) {
        Visualizer::Instance().RecordState("Left-Right Imbalance at " + std::to_string(node -> data) + ". RL Rotating.", 3, captureState(node -> data, -1, RED), code);
        node -> left = leftRotate(node -> left);
        return rightRotate(node);
    }

    //RL
    if (balanceFactor < -1 && value < node -> right -> data) {
        Visualizer::Instance().RecordState("Right-Left Imbalance at " + std::to_string(node -> data) + ". LR Rotating.", 3, captureState(node -> data, -1, RED), code);
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}





