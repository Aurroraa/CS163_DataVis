#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "raylib.h"
#include "../Global.h"

struct GraphEdge {
    int to;
    int weight;
};

struct GraphNode {
    int id;
    int data;
    float x, y;
    float vx, vy;
    bool isFixed;
    bool isSeen;
};

class Graph {
private:
    std::unordered_map<int, GraphNode> vertices;
    std::unordered_map<int, std::vector<GraphEdge>> adjList;
    int nextNodeID = 0;
    bool isDirected = false;

    void addLiveVertex(int id, float startX, float startY, float width, float height);

public:
    Graph() = default;

    void clear();
    void setDirected(bool directed);
    bool getDirected() { return isDirected; }

    void addEdge(int fromId, int toId, int weight = 0);

    void initFromLiveText(std::string text, bool directed, float startX, float startY, float width, float height);
    void updateVertexPosition(int id, float x, float y, float minX, float maxX, float minY, float maxY);

    void updatePhysics(float minX, float minY, float maxX, float maxY);
    void toggleFixed(int id);

    AnimationState captureState(int highlightNode = -1, int highlightEdgeFrom = -1, int highlightEdgeTo = -1, Color c = BLUE);
    int getClickedVertex(float mouseX, float mouseY);

    void runKruskalMST();
    void runDijkstra(int startVertex);
    int getFirstVertexId();
};