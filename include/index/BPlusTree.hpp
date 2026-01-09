/**
 * @file BPlusTree.hpp
 * @brief B+ Tree data structure header for database indexing
 * @author Ece Nur Güner
 * @date 2025
 */

#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include <iostream>
#include "../data_structures/LinkedList.hpp"

class Row;

namespace idx {

    /**
     * @brief Record identifier structure
     *
     * Stores location information for database records.
     * Used as values in B+ tree leaf nodes.
     */
    struct RecordID {
        int page_id;
        int slot_id;
        Row* row_ptr;
        
        /**
         * @brief Default constructor - creates invalid RecordID
         */
        RecordID() : page_id(-1), slot_id(-1), row_ptr(nullptr) {}
        
        /**
         * @brief Parameterized constructor
         * @param pid Page ID
         * @param sid Slot ID
         * @param ptr Pointer to row
         */
        RecordID(int pid, int sid, Row* ptr) : page_id(pid), slot_id(sid), row_ptr(ptr) {}
        
        /**
         * @brief Equality comparison operator
         * @param other Other RecordID to compare
         * @return true if all fields match
         */
        bool operator==(const RecordID& other) const {
            return page_id == other.page_id && 
                   slot_id == other.slot_id &&
                   row_ptr == other.row_ptr;
        }
        
        /**
         * @brief Checks if RecordID is valid
         * @return true if at least one field is set
         */
        bool isValid() const {
            return page_id != -1 || slot_id != -1 || row_ptr != nullptr;
        }
    };

    /**
     * @brief B+ Tree node structure
     *
     * Represents a single node in the B+ tree. Can be either a leaf node
     * (containing key-value pairs) or an internal node (containing keys
     * and child pointers).
     */
    struct BPlusNode {
        bool is_leaf;
        int key_count;
        int max_degree;
        
        int* keys;
        BPlusNode** children;
        RecordID* values;
        
        BPlusNode* next;  ///< Pointer to next leaf node (for leaf chain)
        
        /**
         * @brief Constructs a new B+ tree node
         *
         * Allocates arrays for keys, values (if leaf), or children (if internal).
         *
         * @param degree Maximum number of keys per node
         * @param leaf Whether this is a leaf node
         */
        BPlusNode(int degree, bool leaf = false) {
            this->is_leaf = leaf;
            this->key_count = 0;
            this->max_degree = degree;
            this->next = nullptr;
            
            // +1 extra space for temporary overflow during split/merge
            this->keys = new int[degree];

            if (is_leaf) {
                this->values = new RecordID[degree];
                this->children = nullptr;
            } else {
                this->children = new BPlusNode*[degree + 1];
                for (int i = 0; i <= degree; i++) {
                    this->children[i] = nullptr;
                }
                this->values = nullptr;
            }
        }

        /**
         * @brief Destructor - frees allocated arrays
         */
        ~BPlusNode() {
            delete[] keys;
            if (is_leaf) {
                delete[] values;
            } else {
                delete[] children;
            }
        }
    };

    /**
     * @brief B+ Tree implementation for database indexing
     *
     * Self-balancing tree structure optimized for range queries.
     * All values are stored in leaf nodes, linked together for efficient range scans.
     * Supports insertion, deletion, search, and range queries.
     */
    class BPlusTree {
    private:
        BPlusNode* root;
        int degree;
        int size_;

        void splitChild(BPlusNode* parent, int index, BPlusNode* child);
        bool insertNonFull(BPlusNode* node, int key, RecordID value);

        void removeInternal(BPlusNode* node, int key);
        void removeFromLeaf(BPlusNode* node, int idx);
        void fill(BPlusNode* node, int idx);
        void borrowFromPrev(BPlusNode* node, int idx);
        void borrowFromNext(BPlusNode* node, int idx);
        void merge(BPlusNode* node, int idx);
        
        int findKey(BPlusNode* node, int key);
        BPlusNode* findLeaf(int key) const;
        BPlusNode* findLeftmostLeaf() const;
        void updateParentKeys(BPlusNode* node);

        void removeTree(BPlusNode* node);
        void printTree(BPlusNode* node, int level);

    public:
        /**
         * @brief Constructs a new B+ Tree
         * @param _degree Maximum number of keys per node (default: 4, minimum: 3)
         */
        BPlusTree(int _degree = 4);
        
        /**
         * @brief Destructor - frees all tree memory
         */
        ~BPlusTree();

        /**
         * @brief Inserts a key-value pair into the tree
         * @param key The key to insert
         * @param value The RecordID value associated with the key
         */
        void insert(int key, RecordID value);
        
        /**
         * @brief Removes a key from the tree
         * @param key The key to remove
         */
        void remove(int key);
        
        /**
         * @brief Searches for a key in the tree
         * @param key The key to search for
         * @return RecordID associated with key, or invalid RecordID if not found
         */
        RecordID search(int key);

        /**
         * @brief Prints all keys in the specified range
         * @param minKey Minimum key value (inclusive)
         * @param maxKey Maximum key value (inclusive)
         */
        void searchRange(int minKey, int maxKey);
        
        /**
         * @brief Returns all RecordIDs for keys in the specified range
         * @param minKey Minimum key value (inclusive)
         * @param maxKey Maximum key value (inclusive)
         * @return LinkedList of RecordIDs for keys in range
         */
        LinkedList<RecordID> searchRangeResults(int minKey, int maxKey);
        
        /**
         * @brief Returns all RecordIDs for keys less than maxKey
         * @param maxKey Maximum key value (exclusive)
         * @return LinkedList of RecordIDs for keys < maxKey
         */
        LinkedList<RecordID> rangeLessThan(int maxKey);
        
        /**
         * @brief Returns all RecordIDs for keys greater than minKey
         * @param minKey Minimum key value (exclusive)
         * @return LinkedList of RecordIDs for keys > minKey
         */
        LinkedList<RecordID> rangeGreaterThan(int minKey);
        
        /**
         * @brief Returns all RecordIDs for keys between minKey and maxKey
         * @param minKey Minimum key value (inclusive)
         * @param maxKey Maximum key value (inclusive)
         * @return LinkedList of RecordIDs for keys in range
         */
        LinkedList<RecordID> rangeBetween(int minKey, int maxKey);
        
        /**
         * @brief Checks if the tree is empty
         * @return true if tree is empty, false otherwise
         */
        bool isEmpty() const;
        
        /**
         * @brief Returns the number of key-value pairs in the tree
         * @return Number of key-value pairs
         */
        int size() const;
        
        /**
         * @brief Prints the entire tree structure
         */
        void print();
        
        /**
         * @brief Gets the root node (for testing/debugging)
         * @return Pointer to root node
         */
        BPlusNode* getRoot() { return root; }
    };

}

#endif