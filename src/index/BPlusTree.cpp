/**
 * @file BPlusTree.cpp
 * @brief Implementation of B+ Tree data structure for database indexing
 * @author Ece Güner
 * @date 2025
 */

#include "../../include/index/BPlusTree.hpp"    

namespace index {

    // ==================== Private Helper Methods ====================

    /**
     * @brief Finds the leaf node that should contain the given key
     *
     * Traverses from root to leaf following the B+ tree structure.
     *
     * @pre Tree may be empty (returns nullptr)
     * @post Returns leaf node containing key or nullptr if tree is empty
     *
     * @param key The key to search for
     * @return Pointer to leaf node containing the key, or nullptr if tree is empty
     */
    BPlusNode* BPlusTree::findLeaf(int key) const {
        if (root == nullptr) return nullptr;
        
        BPlusNode* current = root;
        while (!current->is_leaf) {
            int i = 0;
            while (i < current->key_count && key >= current->keys[i]) {
                i++;
            }
            current = current->children[i];
        }
        return current;
    }

    /**
     * @brief Finds the leftmost leaf node in the tree
     *
     * Used for range queries starting from minimum key.
     *
     * @pre Tree may be empty (returns nullptr)
     * @post Returns leftmost leaf node or nullptr if tree is empty
     *
     * @return Pointer to leftmost leaf node, or nullptr if tree is empty
     */
    BPlusNode* BPlusTree::findLeftmostLeaf() const {
        if (root == nullptr) return nullptr;
        
        BPlusNode* current = root;
        while (!current->is_leaf) {
            current = current->children[0];
        }
        return current;
    }

    /**
     * @brief Finds the index position for a key in a node
     *
     * Returns the index where the key should be located or inserted.
     *
     * @pre node is not nullptr
     * @post Returns valid index in range [0, node->key_count]
     *
     * @param node The node to search in
     * @param key The key to find position for
     * @return Index position for the key
     */
    int BPlusTree::findKey(BPlusNode* node, int key) {
        int idx = 0;
        while (idx < node->key_count && node->keys[idx] < key) {
            idx++;
        }
        return idx;
    }

    // ==================== Split Operation ====================

    /**
     * @brief Splits a full child node into two nodes
     *
     * When a node becomes full (key_count == degree - 1), it must be split.
     * For leaf nodes: keys and values are distributed, and leaf chain is updated.
     * For internal nodes: middle key is promoted to parent, children are redistributed.
     *
     * @pre parent is not nullptr
     * @pre child is not nullptr
     * @pre child->key_count == degree - 1 (node is full)
     * @pre index is valid position in parent
     * @post child has approximately half of original keys
     * @post new node created with remaining keys
     * @post parent updated with new separator key
     *
     * @param parent The parent node containing the child
     * @param index The index of the child in parent's children array
     * @param child The full child node to be split
     */
    void BPlusTree::splitChild(BPlusNode* parent, int index, BPlusNode* child) {
        int mid = child->key_count / 2;
        
        BPlusNode* newNode = new BPlusNode(degree, child->is_leaf);
        
        if (child->is_leaf) {
            newNode->key_count = child->key_count - mid;
            
            for (int i = 0; i < newNode->key_count; i++) {
                newNode->keys[i] = child->keys[mid + i];
                newNode->values[i] = child->values[mid + i];
            }
            
            child->key_count = mid;
            
            newNode->next = child->next;
            child->next = newNode;
            
            for (int i = parent->key_count; i > index; i--) {
                parent->keys[i] = parent->keys[i - 1];
                parent->children[i + 1] = parent->children[i];
            }
            
            parent->keys[index] = newNode->keys[0];
            parent->children[index + 1] = newNode;
            parent->key_count++;
        } else {
            newNode->key_count = child->key_count - mid - 1;
            
            for (int i = 0; i < newNode->key_count; i++) {
                newNode->keys[i] = child->keys[mid + 1 + i];
            }
            
            for (int i = 0; i <= newNode->key_count; i++) {
                newNode->children[i] = child->children[mid + 1 + i];
            }
            
            int upKey = child->keys[mid];
            child->key_count = mid;
            
            for (int i = parent->key_count; i > index; i--) {
                parent->keys[i] = parent->keys[i - 1];
                parent->children[i + 1] = parent->children[i];
            }
            
            parent->keys[index] = upKey;
            parent->children[index + 1] = newNode;
            parent->key_count++;
        }
    }

    // ==================== Insert Operations ====================

    /**
     * @brief Inserts a key-value pair into a non-full node
     *
     * Recursively inserts into the appropriate leaf node. If key already exists,
     * updates the value and returns false. Otherwise inserts new key-value pair.
     *
     * @pre node is not nullptr
     * @pre node->key_count < degree - 1 (node is not full)
     * @post Key-value pair inserted or updated in tree
     * @post Returns true if new key inserted, false if key updated
     *
     * @param node The node to insert into (must not be full)
     * @param key The key to insert
     * @param value The RecordID value associated with the key
     * @return true if new key was inserted, false if key already existed (value updated)
     */
    bool BPlusTree::insertNonFull(BPlusNode* node, int key, RecordID value) {
        if (node->is_leaf) {
            for (int i = 0; i < node->key_count; i++) {
                if (node->keys[i] == key) {
                    node->values[i] = value;
                    return false;
                }
            }
            
            int i = node->key_count - 1;
            while (i >= 0 && node->keys[i] > key) {
                node->keys[i + 1] = node->keys[i];
                node->values[i + 1] = node->values[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->values[i + 1] = value;
            node->key_count++;
            return true;
        } else {
            int i = node->key_count - 1;
            while (i >= 0 && node->keys[i] > key) {
                i--;
            }
            i++;
            
            if (node->children[i]->key_count == degree - 1) {
                splitChild(node, i, node->children[i]);
                if (key > node->keys[i]) {
                    i++;
                }
            }
            
            return insertNonFull(node->children[i], key, value);
        }
    }

    // ==================== Delete Operations ====================

    /**
     * @brief Removes a key-value pair from a leaf node at given index
     *
     * Shifts remaining keys and values to fill the gap.
     *
     * @pre node is not nullptr
     * @pre node->is_leaf == true
     * @pre idx is valid index in range [0, node->key_count)
     * @post Key at index removed, remaining keys shifted
     * @post node->key_count decreased by 1
     *
     * @param node The leaf node to remove from
     * @param idx The index of the key to remove
     */
    void BPlusTree::removeFromLeaf(BPlusNode* node, int idx) {
        for (int i = idx; i < node->key_count - 1; i++) {
            node->keys[i] = node->keys[i + 1];
            node->values[i] = node->values[i + 1];
        }
        node->key_count--;
    }

    /**
     * @brief Borrows a key from the previous sibling node
     *
     * When a node has minimum keys and needs more, borrows from left sibling.
     * Updates parent separator key accordingly.
     *
     * @pre node is not nullptr
     * @pre idx > 0 (has left sibling)
     * @pre node->children[idx-1] has more than minimum keys
     * @post One key moved from left sibling to child
     * @post Parent separator key updated
     *
     * @param node The parent node
     * @param idx The index of the child that needs keys
     */
    void BPlusTree::borrowFromPrev(BPlusNode* node, int idx) {
        BPlusNode* child = node->children[idx];
        BPlusNode* sibling = node->children[idx - 1];
        
        if (child->is_leaf) {
            for (int i = child->key_count; i > 0; i--) {
                child->keys[i] = child->keys[i - 1];
                child->values[i] = child->values[i - 1];
            }
            
            child->keys[0] = sibling->keys[sibling->key_count - 1];
            child->values[0] = sibling->values[sibling->key_count - 1];
            
            sibling->key_count--;
            child->key_count++;
            
            node->keys[idx - 1] = child->keys[0];
        } else {
            for (int i = child->key_count; i > 0; i--) {
                child->keys[i] = child->keys[i - 1];
            }
            for (int i = child->key_count + 1; i > 0; i--) {
                child->children[i] = child->children[i - 1];
            }
            
            child->keys[0] = node->keys[idx - 1];
            child->children[0] = sibling->children[sibling->key_count];
            
            node->keys[idx - 1] = sibling->keys[sibling->key_count - 1];
            
            sibling->key_count--;
            child->key_count++;
        }
    }

    /**
     * @brief Borrows a key from the next sibling node
     *
     * When a node has minimum keys and needs more, borrows from right sibling.
     * Updates parent separator key accordingly.
     *
     * @pre node is not nullptr
     * @pre idx < node->key_count (has right sibling)
     * @pre node->children[idx+1] has more than minimum keys
     * @post One key moved from right sibling to child
     * @post Parent separator key updated
     *
     * @param node The parent node
     * @param idx The index of the child that needs keys
     */
    void BPlusTree::borrowFromNext(BPlusNode* node, int idx) {
        BPlusNode* child = node->children[idx];
        BPlusNode* sibling = node->children[idx + 1];
        
        if (child->is_leaf) {
            child->keys[child->key_count] = sibling->keys[0];
            child->values[child->key_count] = sibling->values[0];
            child->key_count++;
            
            for (int i = 0; i < sibling->key_count - 1; i++) {
                sibling->keys[i] = sibling->keys[i + 1];
                sibling->values[i] = sibling->values[i + 1];
            }
            sibling->key_count--;
            
            node->keys[idx] = sibling->keys[0];
        } else {
            child->keys[child->key_count] = node->keys[idx];
            child->children[child->key_count + 1] = sibling->children[0];
            child->key_count++;
            
            node->keys[idx] = sibling->keys[0];
            
            for (int i = 0; i < sibling->key_count - 1; i++) {
                sibling->keys[i] = sibling->keys[i + 1];
            }
            for (int i = 0; i < sibling->key_count; i++) {
                sibling->children[i] = sibling->children[i + 1];
            }
            sibling->key_count--;
        }
    }

    /**
     * @brief Merges a child node with its next sibling
     *
     * When both siblings have minimum keys, merges them into one node.
     * For internal nodes, includes the parent separator key in the merge.
     * Deletes the right sibling after merging.
     *
     * @pre node is not nullptr
     * @pre idx < node->key_count (has right sibling)
     * @pre Both siblings have minimum keys
     * @post Left child contains all keys from both siblings
     * @post Right sibling deleted
     * @post Parent key count decreased by 1
     *
     * @param node The parent node
     * @param idx The index of the left child to merge
     */
    void BPlusTree::merge(BPlusNode* node, int idx) {
        BPlusNode* left = node->children[idx];
        BPlusNode* right = node->children[idx + 1];
        
        if (left->is_leaf) {
            for (int i = 0; i < right->key_count; i++) {
                left->keys[left->key_count + i] = right->keys[i];
                left->values[left->key_count + i] = right->values[i];
            }
            left->key_count += right->key_count;
            left->next = right->next;
        } else {
            left->keys[left->key_count] = node->keys[idx];
            left->key_count++;
            
            for (int i = 0; i < right->key_count; i++) {
                left->keys[left->key_count + i] = right->keys[i];
            }
            for (int i = 0; i <= right->key_count; i++) {
                left->children[left->key_count + i] = right->children[i];
            }
            left->key_count += right->key_count;
        }
        
        for (int i = idx; i < node->key_count - 1; i++) {
            node->keys[i] = node->keys[i + 1];
        }
        for (int i = idx + 1; i < node->key_count; i++) {
            node->children[i] = node->children[i + 1];
        }
        node->children[node->key_count] = nullptr;
        node->key_count--;
        
        delete right;
    }

    /**
     * @brief Ensures a child node has sufficient keys by borrowing or merging
     *
     * Attempts to borrow from siblings first. If siblings also have minimum keys,
     * merges the child with a sibling.
     *
     * @pre node is not nullptr
     * @pre idx is valid child index
     * @pre node->children[idx] has minimum or fewer keys
     * @post Child has more than minimum keys OR merged with sibling
     *
     * @param node The parent node
     * @param idx The index of the child that needs keys
     */
    void BPlusTree::fill(BPlusNode* node, int idx) {
        int minKeys = (degree - 1) / 2;
        if (minKeys < 1) minKeys = 1;
        
        if (idx > 0 && node->children[idx - 1]->key_count > minKeys) {
            borrowFromPrev(node, idx);
        }
        else if (idx < node->key_count && node->children[idx + 1]->key_count > minKeys) {
            borrowFromNext(node, idx);
        }
        else {
            if (idx < node->key_count) {
                merge(node, idx);
            } else {
                merge(node, idx - 1);
            }
        }
    }

    /**
     * @brief Recursively removes a key from the tree
     *
     * Handles deletion from both leaf and internal nodes. Ensures nodes maintain
     * minimum key count by borrowing or merging. Updates separator keys after deletion.
     *
     * @pre node is not nullptr
     * @pre key exists in the tree
     * @post Key removed from tree
     * @post Tree structure maintained (all nodes have sufficient keys)
     * @post size_ decremented
     *
     * @param node The current node in the recursive traversal
     * @param key The key to remove
     */
    void BPlusTree::removeInternal(BPlusNode* node, int key) {
        if (node->is_leaf) {
            for (int i = 0; i < node->key_count; i++) {
                if (node->keys[i] == key) {
                    removeFromLeaf(node, i);
                    size_--;
                    return;
                }
            }
            return;
        }
        
        // Find child index
        int idx = 0;
        while (idx < node->key_count && key >= node->keys[idx]) {
            idx++;
        }
        
        int minKeys = (degree - 1) / 2;
        if (minKeys < 1) minKeys = 1;
        
        // Fill child if it has minimum keys
        if (node->children[idx]->key_count <= minKeys) {
            fill(node, idx);
            
            // Recalculate index after structural change
            idx = 0;
            while (idx < node->key_count && key >= node->keys[idx]) {
                idx++;
            }
            
            // Safety check
            if (idx > node->key_count) {
                idx = node->key_count;
            }
        }
        
        // Descend
        if (node->children[idx] != nullptr) {
            removeInternal(node->children[idx], key);
        }
        
        // Update separator keys after deletion
        for (int i = 0; i < node->key_count; i++) {
            if (node->children[i + 1] != nullptr) {
                BPlusNode* leftmost = node->children[i + 1];
                while (!leftmost->is_leaf) {
                    if (leftmost->children[0] == nullptr) break;
                    leftmost = leftmost->children[0];
                }
                if (leftmost != nullptr && leftmost->key_count > 0) {
                    node->keys[i] = leftmost->keys[0];
                }
            }
        }
    }

    // ==================== Tree Cleanup ====================

    /**
     * @brief Recursively deletes all nodes in the tree
     *
     * Used by destructor to free all allocated memory.
     *
     * @pre node may be nullptr
     * @post All nodes in subtree deleted, memory freed
     *
     * @param node The root of the subtree to delete
     */
    void BPlusTree::removeTree(BPlusNode* node) {
        if (node == nullptr) return;
        
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; i++) {
                removeTree(node->children[i]);
            }
        }
        delete node;
    }

    /**
     * @brief Recursively prints the tree structure
     *
     * Prints tree in hierarchical format with indentation showing levels.
     * [L] indicates leaf nodes, [I] indicates internal nodes.
     *
     * @pre node may be nullptr
     * @post Tree structure printed to stdout
     *
     * @param node The current node to print
     * @param level The current depth level (for indentation)
     */
    void BPlusTree::printTree(BPlusNode* node, int level) {
        if (node == nullptr) return;
        
        for (int i = 0; i < level; i++) {
            std::cout << "  ";
        }
        
        std::cout << (node->is_leaf ? "[L] " : "[I] ");
        std::cout << "(";
        for (int i = 0; i < node->key_count; i++) {
            std::cout << node->keys[i];
            if (i < node->key_count - 1) std::cout << ", ";
        }
        std::cout << ")" << std::endl;
        
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; i++) {
                printTree(node->children[i], level + 1);
            }
        }
    }

    // ==================== Public Methods ====================

    /**
     * @brief Constructs a new B+ Tree
     *
     * Creates an empty B+ tree with specified degree (minimum 3).
     *
     * @pre _degree >= 3 (will be set to 3 if less)
     * @post Empty B+ tree created with root = nullptr
     * @post size_ = 0
     *
     * @param _degree The maximum number of keys per node (minimum 3)
     */
    BPlusTree::BPlusTree(int _degree) : root(nullptr), size_(0) {
        degree = (_degree < 3) ? 3 : _degree;
    }

    /**
     * @brief Destructor - frees all tree memory
     *
     * Recursively deletes all nodes in the tree.
     *
     * @post All nodes deleted, memory freed
     */
    BPlusTree::~BPlusTree() {
        removeTree(root);
        root = nullptr;
    }

    /**
     * @brief Inserts a key-value pair into the tree
     *
     * If key already exists, updates the value. Otherwise creates new entry.
     * Handles root splitting when necessary.
     *
     * @pre None (tree may be empty)
     * @post Key-value pair inserted or updated in tree
     * @post size_ incremented if new key inserted
     *
     * @param key The key to insert
     * @param value The RecordID value associated with the key
     */
    void BPlusTree::insert(int key, RecordID value) {
        if (root == nullptr) {
            root = new BPlusNode(degree, true);
            root->keys[0] = key;
            root->values[0] = value;
            root->key_count = 1;
            size_++;
            return;
        }
        
        if (root->key_count == degree - 1) {
            BPlusNode* newRoot = new BPlusNode(degree, false);
            newRoot->children[0] = root;
            splitChild(newRoot, 0, root);
            root = newRoot;
            
            int i = (key >= newRoot->keys[0]) ? 1 : 0;
            if (insertNonFull(newRoot->children[i], key, value)) {
                size_++;
            }
        } else {
            if (insertNonFull(root, key, value)) {
                size_++;
            }
        }
    }

    /**
     * @brief Removes a key from the tree
     *
     * If key exists, removes it and maintains tree structure.
     * If key doesn't exist, does nothing.
     *
     * @pre None (tree may be empty)
     * @post If key existed, it is removed and size_ decremented
     * @post Tree structure maintained (all nodes have sufficient keys)
     *
     * @param key The key to remove
     */
    void BPlusTree::remove(int key) {
        if (root == nullptr) return;
        
        // First verify key exists
        RecordID found = search(key);
        if (!found.isValid()) return;
        
        removeInternal(root, key);
        
        if (root->key_count == 0) {
            BPlusNode* oldRoot = root;
            if (root->is_leaf) {
                root = nullptr;
            } else if (root->children[0] != nullptr) {
                root = root->children[0];
                oldRoot->children[0] = nullptr;
            } else {
                root = nullptr;
            }
            delete oldRoot;
        }
    }

    /**
     * @brief Searches for a key in the tree
     *
     * Returns the RecordID associated with the key if found.
     *
     * @pre None (tree may be empty)
     * @post Returns RecordID if key found, invalid RecordID otherwise
     *
     * @param key The key to search for
     * @return RecordID associated with key, or invalid RecordID if not found
     */
    RecordID BPlusTree::search(int key) {
        RecordID notFound;
        
        BPlusNode* leaf = findLeaf(key);
        if (leaf == nullptr) return notFound;
        
        for (int i = 0; i < leaf->key_count; i++) {
            if (leaf->keys[i] == key) {
                return leaf->values[i];
            }
        }
        
        return notFound;
    }

    /**
     * @brief Prints all keys in the specified range
     *
     * Searches for all keys between minKey and maxKey (inclusive) and prints them.
     *
     * @pre minKey <= maxKey
     * @post All keys in range printed to stdout
     *
     * @param minKey Minimum key value (inclusive)
     * @param maxKey Maximum key value (inclusive)
     */
    void BPlusTree::searchRange(int minKey, int maxKey) {
        if (root == nullptr) {
            std::cout << "Tree is empty." << std::endl;
            return;
        }
        
        BPlusNode* leaf = findLeaf(minKey);
        
        std::cout << "Range [" << minKey << ", " << maxKey << "]: ";
        bool found = false;
        
        while (leaf != nullptr) {
            for (int i = 0; i < leaf->key_count; i++) {
                if (leaf->keys[i] > maxKey) {
                    if (!found) std::cout << "(empty)";
                    std::cout << std::endl;
                    return;
                }
                if (leaf->keys[i] >= minKey) {
                    std::cout << leaf->keys[i] << " ";
                    found = true;
                }
            }
            leaf = leaf->next;
        }
        
        if (!found) std::cout << "(empty)";
        std::cout << std::endl;
    }

    /**
     * @brief Returns all RecordIDs for keys in the specified range
     *
     * Searches for all keys between minKey and maxKey (inclusive) and returns
     * their associated RecordIDs in a linked list.
     *
     * @pre minKey <= maxKey
     * @post Returns LinkedList containing all RecordIDs in range
     *
     * @param minKey Minimum key value (inclusive)
     * @param maxKey Maximum key value (inclusive)
     * @return LinkedList of RecordIDs for keys in range
     */
    LinkedList<RecordID> BPlusTree::searchRangeResults(int minKey, int maxKey) {
        LinkedList<RecordID> results;
        
        if (root == nullptr) return results;
        
        BPlusNode* leaf = findLeaf(minKey);
        
        while (leaf != nullptr) {
            for (int i = 0; i < leaf->key_count; i++) {
                if (leaf->keys[i] > maxKey) {
                    return results;
                }
                if (leaf->keys[i] >= minKey) {
                    results.push_back(leaf->values[i]);
                }
            }
            leaf = leaf->next;
        }
        
        return results;
    }

    /**
     * @brief Returns all RecordIDs for keys less than maxKey
     *
     * Searches from leftmost leaf and collects all keys < maxKey.
     *
     * @pre None (tree may be empty)
     * @post Returns LinkedList containing all RecordIDs for keys < maxKey
     *
     * @param maxKey Maximum key value (exclusive)
     * @return LinkedList of RecordIDs for keys < maxKey
     */
    LinkedList<RecordID> BPlusTree::rangeLessThan(int maxKey) {
        LinkedList<RecordID> results;
        
        if (root == nullptr) return results;
        
        BPlusNode* leaf = findLeftmostLeaf();
        
        while (leaf != nullptr) {
            for (int i = 0; i < leaf->key_count; i++) {
                if (leaf->keys[i] >= maxKey) {
                    return results;
                }
                results.push_back(leaf->values[i]);
            }
            leaf = leaf->next;
        }
        
        return results;
    }

    /**
     * @brief Returns all RecordIDs for keys greater than minKey
     *
     * Searches from leaf containing minKey and collects all keys > minKey.
     *
     * @pre None (tree may be empty)
     * @post Returns LinkedList containing all RecordIDs for keys > minKey
     *
     * @param minKey Minimum key value (exclusive)
     * @return LinkedList of RecordIDs for keys > minKey
     */
    LinkedList<RecordID> BPlusTree::rangeGreaterThan(int minKey) {
        LinkedList<RecordID> results;
        
        if (root == nullptr) return results;
        
        BPlusNode* leaf = findLeaf(minKey);
        
        while (leaf != nullptr) {
            for (int i = 0; i < leaf->key_count; i++) {
                if (leaf->keys[i] > minKey) {
                    results.push_back(leaf->values[i]);
                }
            }
            leaf = leaf->next;
        }
        
        return results;
    }

    /**
     * @brief Returns all RecordIDs for keys between minKey and maxKey
     *
     * Convenience method that calls searchRangeResults.
     *
     * @pre minKey <= maxKey
     * @post Returns LinkedList containing all RecordIDs in range
     *
     * @param minKey Minimum key value (inclusive)
     * @param maxKey Maximum key value (inclusive)
     * @return LinkedList of RecordIDs for keys in range
     */
    LinkedList<RecordID> BPlusTree::rangeBetween(int minKey, int maxKey) {
        return searchRangeResults(minKey, maxKey);
    }

    /**
     * @brief Checks if the tree is empty
     *
     * @pre None
     * @post Returns true if tree has no elements, false otherwise
     *
     * @return true if tree is empty, false otherwise
     */
    bool BPlusTree::isEmpty() const {
        return root == nullptr || size_ == 0;
    }

    /**
     * @brief Returns the number of key-value pairs in the tree
     *
     * @pre None
     * @post Returns current size of tree
     *
     * @return Number of key-value pairs in the tree
     */
    int BPlusTree::size() const {
        return size_;
    }

    /**
     * @brief Prints the entire tree structure
     *
     * Prints tree hierarchy and leaf chain for visualization.
     *
     * @pre None (tree may be empty)
     * @post Tree structure printed to stdout
     */
    void BPlusTree::print() {
        if (root == nullptr) {
            std::cout << "Empty B+ Tree" << std::endl;
            return;
        }
        
        std::cout << "========================================" << std::endl;
        std::cout << "B+ Tree (degree=" << degree << ", size=" << size_ << ")" << std::endl;
        std::cout << "========================================" << std::endl;
        
        printTree(root, 0);
        
        std::cout << "Leaf chain: ";
        BPlusNode* leaf = findLeftmostLeaf();
        while (leaf != nullptr) {
            std::cout << "[";
            for (int i = 0; i < leaf->key_count; i++) {
                std::cout << leaf->keys[i];
                if (i < leaf->key_count - 1) std::cout << ",";
            }
            std::cout << "]";
            if (leaf->next != nullptr) std::cout << " -> ";
            leaf = leaf->next;
        }
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
    }

} // namespace index
