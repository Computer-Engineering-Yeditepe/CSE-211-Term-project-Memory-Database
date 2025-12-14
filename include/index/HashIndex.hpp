#ifndef HASH_INDEX_HPP
#define HASH_INDEX_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>

// Forward declaration
struct Row;

template<typename K>
class HashIndex {
private:
    // "Person 2" Node Structure
    struct Node {
        K key;
        Row* row;
        Node* next;

        Node(K k, Row* r) : key(k), row(r), next(nullptr) {}
    };

    Node** buckets;
    size_t capacity;
    size_t size;
    const float LOAD_FACTOR = 0.75f;

    size_t getHash(const K& key, size_t cap) const;
    void resize();

public:
    explicit HashIndex(size_t initialCapacity = 16);
    ~HashIndex();

    // Delete copy constructors (Rule of 5)
    HashIndex(const HashIndex&) = delete;
    HashIndex& operator=(const HashIndex&) = delete;

    void insert(K key, Row* row);
    void remove(const K& key);
    Row* search(const K& key);

    size_t getSize() const;
    size_t getCapacity() const;
};

// CRITICAL: This path assumes you run the compiler from the project root folder.
#include "../../src/index/HashIndex.cpp"

#endif // HASH_INDEX_HPP