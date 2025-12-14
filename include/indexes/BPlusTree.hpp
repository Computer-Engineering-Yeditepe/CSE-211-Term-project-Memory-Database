#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include "../core/Row.hpp"
#include "../core/Cell.hpp"

struct BPNode {
    bool isLeaf;
    int numKeys;
    
    Cell* keys;       
    BPNode** children; 
    Row** values;      
    
    BPNode* next;    

    BPNode(int maxDegree, bool leaf);
    ~BPNode();
};

class BPlusTree {
private:
    BPNode* root;
    int degree; 

    void splitLeaf(BPNode* parent, int index, BPNode* leaf);
    void splitInternal(BPNode* parent, int index, BPNode* internalNode);
    void insertNonFull(BPNode* node, const Cell& key, Row* value);
    // traverse sildik

public:
    BPlusTree(int deg = 3);
    ~BPlusTree();

    void insert(const Cell& key, Row* value);
    Row* search(const Cell& key);
    void print(); 
};

#endif