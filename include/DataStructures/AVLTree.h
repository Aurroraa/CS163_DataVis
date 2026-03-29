#pragma once
#include <vector>
#include "Global.h"

struct AVLNode {
    int data;
    int height;
    AVLNode* left;
    AVLNode* right;

    int id;
    float x, y;

    AVLNode(int val, int nodeId) : data(val), height(1), left(NULL), right(NULL), id(nodeId), x(0), y(0) {}
};

class AVLTree {
private:
    AVLNode* root = nullptr;
    int nextNodeID = 0;
    int nodeCount = 0;

    // --- Math & Logic ---
    int height(AVLNode* N);
    int getBalanceFactor(AVLNode* N);
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);
    AVLNode* insertNode(AVLNode* node, int data);
    void destroyTree(AVLNode* node);

    // --- Visual Helpers ---
    void calcPositions(AVLNode* node, float x, float y, float hSpacing);
    void collectVisualNodes(AVLNode* node, std::vector<VisualNode>& visuals, int h1, int h2, Color c);

    AVLNode* minValueNode(AVLNode* node);
    AVLNode* deleteNodeHelper(AVLNode* root, int value);

public:
    AVLTree() = default;
    ~AVLTree();

    void init(std::vector<int> values);
    void insert(int value);
    void clear();

    void deleteNode(int value);
    void updateNode(int oldVal, int newVal);
    void searchNode(int value);

    // 🐛 MOVED HERE: Now AVLState.cpp is allowed to use it!
    std::vector<VisualNode> captureState(int h1 = -1, int h2 = -1, Color c = BLUE);
};