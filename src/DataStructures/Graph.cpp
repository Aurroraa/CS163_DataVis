#include "DataStructures/Graph.h"
#include "Visualizer.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <map>

void Graph::clear() {
    vertices.clear();
    adjList.clear();
    nextNodeID = 0;
}

void Graph::setDirected(bool directed) {
    isDirected = directed;
}

void Graph::updateVertexPosition(int id, float x, float y, float minX, float maxX, float minY, float maxY) {
    if (vertices.count(id)) {
        vertices[id].x = std::clamp(x, minX, maxX);
        vertices[id].y = std::clamp(y, minY, maxY);
        vertices[id].isFixed = true;
    }
}

void Graph::addEdge(int fromId, int toId, int weight) {
    if (fromId == toId) return;

    for (const auto& edge : adjList[fromId]) {
        if (edge.to == toId) return;
    }

    adjList[fromId].push_back({toId, weight});
    if (!isDirected) {
        adjList[toId].push_back({fromId, weight});
    }
}

void Graph::addLiveVertex(int id, float startX, float startY, float width, float height) {
    if (vertices.find(id) == vertices.end()) {
        float rx = startX + 40.0f + (rand() % (int)(width - 80.0f));
        float ry = startY + 40.0f + (rand() % (int)(height - 80.0f));
        vertices[id] = {id, id, rx, ry, 0.0f, 0.0f, false, true};
    } else {
        vertices[id].isSeen = true;
    }
}

void Graph::initFromLiveText(std::string text, bool directed, float startX, float startY, float width, float height) {
    setDirected(directed);
    adjList.clear();

    for(auto& pair : vertices) pair.second.isSeen = false;

    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        std::stringstream lineStream(line);
        std::string token;
        std::vector<std::string> tokens;
        while (lineStream >> token) tokens.push_back(token);

        try {
            if (tokens.size() == 1) {
                int u = std::stoi(tokens[0]);
                addLiveVertex(u, startX, startY, width, height);
            }
            else if (tokens.size() == 2) {
                int u = std::stoi(tokens[0]);
                int v = std::stoi(tokens[1]);
                addLiveVertex(u, startX, startY, width, height);
                addLiveVertex(v, startX, startY, width, height);
                addEdge(u, v, 0);
            }
            else if (tokens.size() >= 3) {
                int u = std::stoi(tokens[0]);
                int v = std::stoi(tokens[1]);
                int w = std::stoi(tokens[2]);
                addLiveVertex(u, startX, startY, width, height);
                addLiveVertex(v, startX, startY, width, height);
                addEdge(u, v, w);
            }
        } catch (...) {

        }
    }

    for(auto it = vertices.begin(); it != vertices.end(); ) {
        if(!it->second.isSeen) it = vertices.erase(it);
        else ++it;
    }

    Visualizer::Instance().RecordState("", 0, captureState(), {});
}

int Graph::getClickedVertex(float mouseX, float mouseY) {
    for (const auto& pair : vertices) {
        float dx = pair.second.x - mouseX;
        float dy = pair.second.y - mouseY;
        // Check if the click is inside the circle (radius 26)
        if (std::sqrt(dx*dx + dy*dy) <= 26.0f) {
            return pair.first;
        }
    }
    return -1;
}

// 🌟 1. ADD THIS ANYWHERE: Toggles the fixed state
void Graph::toggleFixed(int id) {
    if (vertices.count(id)) {
        vertices[id].isFixed = !vertices[id].isFixed;
    }
}
void Graph::updatePhysics(float minX, float minY, float maxX, float maxY) {
    float dt = 0.5f;
    float centerX = minX + (maxX - minX) / 2.0f;
    float centerY = minY + (maxY - minY) / 2.0f;

    // 1. Repulsion (Soft push so they don't blast apart)
    for (auto& p1 : vertices) {
        for (auto& p2 : vertices) {
            if (p1.first == p2.first) continue;
            float dx = p1.second.x - p2.second.x;
            float dy = p1.second.y - p2.second.y;
            float distSq = dx*dx + dy*dy;

            if (distSq < 1.0f) { dx = 1.0f; dy = 1.0f; distSq = 2.0f; }

            if (distSq < 80000.0f) {
                float force = 400.0f / distSq; // 🌟 Much softer push
                p1.second.vx += dx * force;
                p1.second.vy += dy * force;
            }
        }
    }

    // 2. Attraction (Gentle springs)
    for (const auto& pair : adjList) {
        int from = pair.first;
        for (const auto& edge : pair.second) {
            int to = edge.to;
            if (vertices.count(from) && vertices.count(to)) {
                float dx = vertices[to].x - vertices[from].x;
                float dy = vertices[to].y - vertices[from].y;
                float dist = std::sqrt(dx*dx + dy*dy);

                if (dist > 0.0f) {
                    float force = (dist - 130.0f) * 0.005f; // 🌟 Springs are 3x weaker
                    vertices[from].vx += (dx / dist) * force;
                    vertices[from].vy += (dy / dist) * force;
                    vertices[to].vx -= (dx / dist) * force;
                    vertices[to].vy -= (dy / dist) * force;
                }
            }
        }
    }

    // 3. Gravity, Velocity & Anti-Twitch
    for (auto& pair : vertices) {
        GraphNode& n = pair.second;
        if (!n.isFixed) {
            n.vx += (centerX - n.x) * 0.001f; // Barely any gravity
            n.vy += (centerY - n.y) * 0.001f;

            n.x += n.vx * dt;
            n.y += n.vy * dt;

            // 🌟 EXTREME FRICTION: Feels like moving through thick liquid
            n.vx *= 0.45f;
            n.vy *= 0.45f;

            // 🌟 AGGRESSIVE ANTI-TWITCH: Go to sleep much earlier
            if (std::abs(n.vx) < 1.0f) n.vx = 0.0f;
            if (std::abs(n.vy) < 1.0f) n.vy = 0.0f;

            // Wall collision (almost zero bounce)
            if (n.x < minX + 30.0f) { n.x = minX + 30.0f; n.vx *= -0.1f; }
            if (n.x > maxX - 30.0f) { n.x = maxX - 30.0f; n.vx *= -0.1f; }
            if (n.y < minY + 30.0f) { n.y = minY + 30.0f; n.vy *= -0.1f; }
            if (n.y > maxY - 30.0f) { n.y = maxY - 30.0f; n.vy *= -0.1f; }
        } else {
            n.vx = 0.0f;
            n.vy = 0.0f;
        }
    }
}

AnimationState Graph::captureState(int highlightNode, int highlightEdgeFrom, int highlightEdgeTo, Color c) {
    AnimationState state;

    state.isDirected = this->isDirected;
    for (const auto& pair : vertices) {
        VisualNode v;
        v.id = pair.second.id; v.data = pair.second.data;
        v.x = pair.second.x; v.y = pair.second.y;
        v.drawX = v.x; v.drawY = v.y;
        v.color = (v.id == highlightNode) ? c : BLUE;

        // Hijack highlightIndex to tell the renderer if it's Pinned!
        v.highlightIndex = pair.second.isFixed ? 1 : 0;

        state.nodes.push_back(v);
    }

    for (const auto& pair : adjList) {
        int from = pair.first;
        for (const auto& edge : pair.second) {
            if (!isDirected && from > edge.to) continue;

            VisualEdge ve;
            ve.fromId = from; ve.toId = edge.to; ve.weight = edge.weight;
            ve.isDirected = isDirected;

            if ((from == highlightEdgeFrom && edge.to == highlightEdgeTo) ||
                (!isDirected && from == highlightEdgeTo && edge.to == highlightEdgeFrom)) {
                ve.color = c;
                } else ve.color = DARKGRAY;

            state.edges.push_back(ve);
        }
    }
    return state;
}

struct DSU {
    std::unordered_map<int, int> parent;
    void make_set(int v){parent[v] = v;}
    int find_set(int v) {
        if (v == parent[v]) return v;
        return parent[v] = find_set(parent[v]);
    }
    void union_sets(int a, int b) {
        a = find_set(a);
        b = find_set(b);
        if (a != b) parent[b] = a;
    }
};

void Graph::runKruskalMST() {
    if (vertices.empty()) return;

    std::vector<std::string> code = {
        "Sort all edges by weight",
        "For each edge(u, v) in sorted edges:",
        "    If Find(u) != Find(v):",
        "        Add edge to MST",
        "        Union(u, v)"
    };

    struct Edge{int u, v, w;};
    std::vector<Edge> edges;
    for (const auto& pair:adjList) {
        int u = pair.first;
        for (const auto& edge : pair.second) {
            if (isDirected || u < edge.to) {
                edges.push_back({u, edge.to, edge.weight});
            }
        }
    }
    std::sort(edges.begin(), edges.end(), [](Edge a, Edge b){return a.w < b.w;});

    DSU dsu;
    for (const auto& pair : vertices) {
        dsu.make_set(pair.first);
    }

    std::vector<std::pair<int, int>> mstEdges;

    auto getCurState = [&](int curU = -1, int curV = -1, Color curColor = BLUE, bool hideUnused = false) {
        AnimationState state = captureState();
        std::vector<VisualEdge> filteredEdges; // Temporary list

        for (auto& ve : state.edges) {
            bool inMST = false;
            for (auto& mstE : mstEdges) {
                if ((ve.fromId == mstE.first && ve.toId == mstE.second) ||
                    (!isDirected && ve.fromId == mstE.second && ve.toId == mstE.first)) {
                    inMST = true; break;
                    }
            }

            if (inMST) {
                ve.color = GREEN; // Locked into MST
                filteredEdges.push_back(ve);
            } else if ((ve.fromId == curU && ve.toId == curV) ||
                       (!isDirected && ve.fromId == curV && ve.toId == curU)) {
                ve.color = curColor; // Currently evaluating this edge
                filteredEdges.push_back(ve);
                       } else if (!hideUnused) {
                           // Only keep the standard grey edges if we aren't hiding them!
                           filteredEdges.push_back(ve);
                       }
        }

        state.edges = filteredEdges; // Overwrite with the filtered list
        return state;
    };
    Visualizer::Instance().RecordState("", 0, getCurState(), code);

    // 3. Evaluate edges
    for (const auto& e : edges) {
        Visualizer::Instance().RecordState("", 1, getCurState(e.u, e.v, ORANGE), code);

        if (dsu.find_set(e.u) != dsu.find_set(e.v)) {
            Visualizer::Instance().RecordState("", 3, getCurState(e.u, e.v, GREEN), code);
            mstEdges.push_back({e.u, e.v});
            dsu.union_sets(e.u, e.v);
            Visualizer::Instance().RecordState("", 4, getCurState(), code);
        } else {
            Visualizer::Instance().RecordState("", 2, getCurState(e.u, e.v, RED), code);
        }
    }
    // ... end of your Kruskal loop ...

    // 🌟 THE FIX: Manually build the final frame to forcefully drop unused edges
    AnimationState finalState = captureState();
    std::vector<VisualEdge> finalEdges;

    for (auto& ve : finalState.edges) {
        // Check if this edge is in our MST list
        bool inMST = false;
        for (auto& mstE : mstEdges) {
            if ((ve.fromId == mstE.first && ve.toId == mstE.second) ||
                (!isDirected && ve.fromId == mstE.second && ve.toId == mstE.first)) {
                inMST = true;
                break;
                }
        }

        // ONLY add it to the final frame if it is part of the MST!
        if (inMST) {
            ve.color = GREEN;
            finalEdges.push_back(ve);
        }
    }

    finalState.edges = finalEdges; // Overwrite the edges list
    Visualizer::Instance().RecordState("", -1, finalState, code);
}

int Graph::getFirstVertexId() {
    if (vertices.empty()) return -1;
    return vertices.begin()->first;
}

void Graph::runDijkstra(int startVertex) {
    if (vertices.find(startVertex) == vertices.end()) return;

    std::vector<std::string> code = {
        "dist[src] = 0, others = INF",
        "While unvisited nodes exist:",
        "  u = unvisited node with min dist",
        "  For each neighbor v of u:",
        "    dist[v] = min(dist[v], dist[u] + w)"
    };

    std::map<int, bool> known;
    std::map<int, int> dist;
    std::map<int, int> path;

    for (const auto&p : vertices) {
        known[p.first] = false;
        dist[p.first] = 1e9;
        path[p.first] = -1;
    }
    dist[startVertex] = 0;

    auto buildTable = [&]() {
        std::vector<std::vector<std::string>> table;
        table.push_back({"V", "Known", "Dist", "Path"}); // Header
        for (const auto& p : vertices) {
            int v = p.first;
            std::string k = known[v] ? "T" : "F";
            std::string d = (dist[v] == 1e9) ? "INF" : std::to_string(dist[v]);
            std::string p_str = (path[v] == -1) ? "-" : std::to_string(path[v]);
            table.push_back({std::to_string(v), k, d, p_str});
        }
        return table;
    };

    auto getCurState = [&](int activeNode = -1, int edgeU = -1, int edgeV = -1, Color c = ORANGE) {
        AnimationState state = captureState(activeNode, edgeU, edgeV, c);
        state.table = buildTable();

        // Color locked-in shortest paths green
        for (auto& ve : state.edges) {
            for (const auto& p : vertices) {
                int v = p.first;
                int parent = path[v];
                if (parent != -1) {
                    if ((ve.fromId == parent && ve.toId == v) || (!isDirected && ve.fromId == v && ve.toId == parent)) {
                        // Don't overwrite the active highlighted edge
                        if (ve.color.r == DARKGRAY.r && ve.color.g == DARKGRAY.g && ve.color.b == DARKGRAY.b) {
                            ve.color = GREEN;
                        }
                    }
                }
            }
        }
        return state;
    };

    Visualizer::Instance().RecordState("", 0, getCurState(startVertex), code);

    for (int i = 0; i < vertices.size(); i++) {
        int v = -1;
        int minDist = 1e9;
        for (const auto& p : vertices) {
            if (!known[p.first] && dist[p.first] < minDist) {
                minDist = dist[p.first];
                v = p.first;
            }
        }

        if (v == -1 || minDist == 1e9) break;

        Visualizer::Instance().RecordState("", 2, getCurState(v), code);

        known[v] = true;
        Visualizer::Instance().RecordState("", 3, getCurState(v), code);

        for (const auto& edge:adjList[v]) {
            int u = edge.to;
            int weight = edge.weight;

            if (!known[u]) {
                Visualizer::Instance().RecordState("", 3, getCurState(v, v, u, ORANGE), code);

                if (dist[v] + weight < dist[u]) {
                    dist[u] = dist[v] + weight;
                    path[u] = v;
                    Visualizer::Instance().RecordState("", 4, getCurState(u, v, u, GREEN), code);
                }
                else {
                    Visualizer::Instance().RecordState("", 4, getCurState(v, v, u, RED), code);
                }
            }
        }
    }
    AnimationState finalState = getCurState();

    for (const auto& p : vertices) {
        int target = p.first;
        if (target == startVertex) continue;

        if (dist[target] == 1e9) {
            finalState.finalPaths.push_back(std::to_string(startVertex) + " -> " + std::to_string(target) + " : Unreachable");
            continue;
        }

        std::vector<int> currPath;
        int curr = target;
        while (curr != -1) {
            currPath.push_back(curr);
            curr = path[curr];
        }

        std::string pathStr = "";
        for (int i = currPath.size() - 1; i >= 0; i--) {
            pathStr += std::to_string(currPath[i]);
            if (i > 0) pathStr += " -> ";
        }

        finalState.finalPaths.push_back(pathStr + "  (Cost: " + std::to_string(dist[target]) + ")");
    }

    Visualizer::Instance().RecordState("", -1, finalState, code);
}










