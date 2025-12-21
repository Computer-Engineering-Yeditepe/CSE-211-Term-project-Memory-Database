#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include <iostream>
#include <algorithm>

class Row;

namespace index {

    
    struct RecordID {
        
        int page_id;
        int slot_id;
        Row* row_ptr;
        
        bool operator==(const RecordID& other) const {
            return page_id == other.page_id && 
            slot_id == other.slot_id &&
            row_ptr == other.row_ptr;
        }
    };

   
    struct BPlusNode {
       
        bool is_leaf;
        int key_count;
        int max_degree;
        
        int* keys;
        BPlusNode** children;
        RecordID* values;
        
        
        
        BPlusNode* next;
        
        BPlusNode(int degree, bool leaf = false) {
            
            this->is_leaf = leaf;
            this->key_count = 0;
            this->max_degree = degree;
            this->next = nullptr;
            this->keys = new int[degree - 1];

            if (is_leaf) {
                this->values = new RecordID[degree - 1];
                this->children = nullptr;
            }
            else {
                this->children = new BPlusNode*[degree];
                this->values = nullptr;
            }
        }

        
        ~BPlusNode() {
            
            delete[] keys;
            if (is_leaf) {
                delete[] values;
            } else {
                delete[] children;
            }
        }
    };

    
    class BPlusTree {
    private:
        BPlusNode* root;
        int degree;

        
        void splitChild(BPlusNode* parent, int index, BPlusNode* child);
        void insertNonFull(BPlusNode* node, int key, RecordID value);

        void removeInternal(BPlusNode* node, int key);
        void removeFromLeaf(BPlusNode* node, int idx);
        void removeFromNonLeaf(BPlusNode* node, int idx);
        int getPred(BPlusNode* node, int idx);       // Önceki elemanı bul
        int getSucc(BPlusNode* node, int idx);       // Sonraki elemanı bul
        void fill(BPlusNode* node, int idx);         // Boşalan düğümü doldur
        void borrowFromPrev(BPlusNode* node, int idx); // Soldan ödünç al
        void borrowFromNext(BPlusNode* node, int idx); // Sağdan ödünç al
        void merge(BPlusNode* node, int idx);        // İki düğümü birleştir

        void removeTree(BPlusNode* node);
        void printTree(BPlusNode* node, int level);


    public:
        
        BPlusTree(int _degree = 4);

        
        ~BPlusTree();

        
        void insert(int key, RecordID value);

        void remove(int key);
        
        RecordID search(int key);

        void searchRange(int minKey, int maxKey);
        
        void print();
        
        
        BPlusNode* getRoot() { return root; }
    };

} 











#endif