#pragma once
#include <vector>
#include <string>
#include "Global.h"

struct TrieNode {
    int id;
    int data;
    bool isEndOfWord;
    TrieNode* children[26];

    float x, y;
    int parentId;
    TrieNode(int charData, int nodeId, int pId) {
        data = charData;
        parentId = pId;
        id = nodeId;
        isEndOfWord = false;
        for (int i = 0; i < 26; i++) children[i] = NULL;
        x = 0;
        y = 0;
    }
};

class Trie {
private:
    TrieNode* root;
    int nextNodeID;
    float currentX;

    void destroyTree(TrieNode* node);
    void calcPositions(TrieNode* node, int depth);
    void collectVisualNodes(TrieNode* node, std::vector<VisualNode>& visuals, int highlightId, Color c, float shiftX);

    bool deleteHelper(TrieNode* node, const std::string& word, int depth, std::vector<std::string>& code);
public:
    Trie();
    ~Trie();

    void init(std::vector<std::string> words);
    void clear();
    void insert(std::string word);
    void deleteWord(std::string word);
    void searchWord(std::string word);
    void updateWord(std::string word, int newSize);

    std::vector<VisualNode> captureState(int highlightId = -1, Color c = BLUE);
};