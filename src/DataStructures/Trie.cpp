#include "DataStructures/Trie.h"
#include "Visualizer.h"
#include <algorithm>
#include <cctype>

using namespace std;

Trie::Trie() {
    root = new TrieNode(0, 0, -1);
    nextNodeID = 1;
}

Trie::~Trie() {clear();}

void Trie::destroyTree(TrieNode *node) {
    if (!node) return;
    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            destroyTree(node->children[i]);
        }
    }
    delete node;
}

void Trie::calcPositions(TrieNode *node, int depth) {
    if (!node) return;
    bool isLeaf = true;
    float sumX = 0;
    int childCound = 0;

    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            isLeaf = false;
            calcPositions(node->children[i], depth + 1);
            sumX += node->children[i]->x;
            childCound++;
        }
    }
    if (isLeaf) {
        node->x = currentX;
        currentX += 70.0f;
    }
    else {
        node->x = sumX / childCound;
        currentX = max(currentX, node->x + 70.f);
    }
    node->y = 120.0f + depth * 80.0f;
}

void Trie::collectVisualNodes(TrieNode *node, std::vector<VisualNode> &visuals, int highlightId, Color c, float shiftX) {
    if (!node) return;

    VisualNode v;
    v.id = node->id;
    v.data = node -> data;
    v.x = node->x + shiftX;
    v.y = node->y;

    v.highlightIndex = node->isEndOfWord ? 1:0;
    v.prevNodeIndex = node->parentId;
    v.nextNodeIndex = -1;

    if (node->id == highlightId) v.color = c;
    else v.color = BLUE;

    visuals.push_back(v);

    for (int i = 0; i < 26; i++) {
        if (node->children[i]) collectVisualNodes(node->children[i], visuals, highlightId, c, shiftX);
    }
}

vector<VisualNode> Trie::captureState(int highlightId, Color c) {
    vector<VisualNode> visuals;
    currentX = 0.f;
    calcPositions(root, 0);

    float shiftX = (GetScreenWidth() - currentX)/2.0f;
    if (shiftX < 50.0f) shiftX = 50.0f;

    collectVisualNodes(root, visuals, highlightId, c, shiftX);
    return visuals;
}

void Trie::init(std::vector<std::string> words) {
    clear();
    Visualizer::Instance().RecordState("Cleared Trie", 0, captureState(), {});
    for (const string &word : words) {insert(word);}
}

void Trie::insert(std::string word) {
    string cleanWord = "";
    for (char c:word) if (isalpha(c)) cleanWord += tolower(c);
    if (cleanWord.length() == 0) return;

    vector<string> code = {
        "curr = root",
        "for char in word:",
        "    if curr->children[char] == null:",
        "        curr->children[char] = new Node()",
        "    curr = curr->children[char]",
        "curr->isEndOfWord = true"
    };

    TrieNode *curr = root;
    Visualizer::Instance().RecordState("Inserting word: " + cleanWord, 0, captureState(curr->id, ORANGE), code);

    for (char c:cleanWord) {
        int index = c - 'a';
        if (!curr -> children[index]) {
            curr -> children[index] = new TrieNode(c, nextNodeID++, curr->id);
            Visualizer::Instance().RecordState("Created node for '" + string(1, c) + "'", 3, captureState(curr->children[index]->id, GREEN), code);
        }
        curr = curr -> children[index];
        Visualizer::Instance().RecordState("Traversing to '" + string(1, c) + "'", 4, captureState(curr->id, ORANGE), code);
    }

    curr -> isEndOfWord = true;
    Visualizer::Instance().RecordState("Marking '" + string(1, cleanWord.back()) + "' as End of Word", 5, captureState(curr->id, PURPLE), code );
}

void Trie::searchWord(std::string word) {
    string cleanWord = "";
    for (char c:word) if (isalpha(c)) cleanWord += tolower(c);
    if (cleanWord.length() == 0) return;

    vector<string> code = {
        "curr = root",
        "for char in word:",
        "    if curr->children[char] == null: return false",
        "   curr = curr->children[char]",
        "return curr->isEndOfWord"
    };

    TrieNode *curr = root;
    Visualizer::Instance().RecordState("Searching for word: " + cleanWord, 0, captureState(curr->id, ORANGE), code);
    for (char c:cleanWord) {
        int index = c - 'a';
        if (!curr -> children[index]) {
            Visualizer::Instance().RecordState("Path broken at '" + string(1, c) + "'. NOT FOUND.", 2, captureState(curr->id, RED), code);
            return;
        }
        curr = curr -> children[index];
        Visualizer::Instance().RecordState("Traversing to '" + string(1, c) + "'", 3, captureState(curr->id, ORANGE), code);
    }

    if (curr->isEndOfWord) {
        Visualizer::Instance().RecordState("Word '" + cleanWord + "' FOUND!", 4, captureState(curr->id, GREEN), code);
    }
    else {
        Visualizer::Instance().RecordState("Path exists, but not marked as End of Word. NOT FOUND.", 4, captureState(curr->id, RED), code);
    }
}

void Trie::deleteWord(std::string word) {
    string cleanWord = "";
    for (char c : word) if (isalpha(c)) cleanWord += tolower(c);
    if (cleanWord.empty()) return;

    vector<std::string> code = {
        "if curr is end of word: unmark it",
        "if curr has no children: delete node",
        "return to parent and repeat"
    };

    deleteHelper(root, cleanWord, 0, code);
    Visualizer::Instance().RecordState("Deletion process finished.", -1, captureState(), code);
}

bool Trie::deleteHelper(TrieNode *node, const std::string &word, int depth, std::vector<std::string> &code) {
    if (!node) return false;

    if (depth == word.length()) {
        if (node->isEndOfWord) {
            node->isEndOfWord = false;
            Visualizer::Instance().RecordState("Unmarked End of Word flag", 0, captureState(node->id, ORANGE), code);
        }

        for (int i = 0; i < 26; i++) {
            if (node->children[i]) return false;
        }
        return true;
    }

    int index = word[depth] - 'a';
    if (deleteHelper(node->children[index], word, depth + 1, code)) {
        Visualizer::Instance().RecordState("Deleting unused node '" + string(1, word[depth]) + "'", 1, captureState(node->children[index]->id, RED), code);
        delete node->children[index];
        node->children[index] = nullptr;

        if (node->isEndOfWord) return false;
        for (int i = 0; i < 26; i++) if (node->children[i]) return false;
        return true;
    }
    return false;
}


void Trie::updateWord(std::string word, int newSize) {
    std::string cleanWord = "";
    for (char c : word) if (isalpha(c)) cleanWord += tolower(c);

    // Safety check: Don't allow invalid sizes
    if (cleanWord.empty() || newSize <= 0 || newSize > cleanWord.length()) {
        Visualizer::Instance().RecordState("Invalid update parameters!", -1, captureState(), {});
        return;
    }

    std::string newWord = cleanWord.substr(0, newSize);

    std::vector<std::string> code = {
        "deleteWord(originalWord)",
        "insert(truncatedWord)"
    };

    Visualizer::Instance().RecordState("Starting Update: Deleting '" + cleanWord + "'", 0, captureState(), code);
    deleteWord(cleanWord);

    Visualizer::Instance().RecordState("Update Phase 2: Inserting '" + newWord + "'", 1, captureState(), code);
    insert(newWord);
}






