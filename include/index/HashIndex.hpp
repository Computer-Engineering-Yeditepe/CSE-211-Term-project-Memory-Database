#ifndef HASH_INDEX_HPP
#define HASH_INDEX_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>



// Forward declaration

class Row;
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





template <typename K>
size_t HashIndex<K>::getHash(const K& key, size_t cap) const {
    return std::hash<K>{}(key) % cap;
}



template <typename K>
void HashIndex<K>::resize() {
    size_t newCapacity = capacity * 2;
    Node** newBuckets = new Node*[newCapacity];

    for (size_t i = 0; i < newCapacity; ++i) {
        newBuckets[i] = nullptr;
    }

    for (size_t i = 0; i < capacity; ++i) {
        Node* current = buckets[i];
        while (current != nullptr) {

            Node* nextNode = current->next;
            size_t newIndex = getHash(current->key, newCapacity);

            current->next = newBuckets[newIndex];
            newBuckets[newIndex] = current;
            current = nextNode;

        }

    }

    delete[] buckets;
    buckets = newBuckets;
    capacity = newCapacity;

}



template <typename K>
HashIndex<K>::HashIndex(size_t initialCapacity) : capacity(initialCapacity), size(0) {
    buckets = new Node*[capacity];
    for (size_t i = 0; i < capacity; ++i) {
        buckets[i] = nullptr;
    }

}



template <typename K>
HashIndex<K>::~HashIndex() {

    for (size_t i = 0; i < capacity; ++i) {
        Node* current = buckets[i];
        while (current != nullptr) {

            Node* temp = current;
            current = current->next;
            delete temp;

        }

    }
    delete[] buckets;
}



template <typename K>
void HashIndex<K>::insert(K key, Row* row) {

    if (static_cast<float>(size) / capacity >= LOAD_FACTOR) {
        resize();
    }

    size_t index = getHash(key, capacity);
    Node* newNode = new Node(key, row);
    newNode->next = buckets[index];
    buckets[index] = newNode;
    size++;

}



template <typename K>
Row* HashIndex<K>::search(const K& key) {

    size_t index = getHash(key, capacity);
    Node* current = buckets[index];
    while (current != nullptr) {

        if (current->key == key) return current->row;
        current = current->next;

    }

    return nullptr;
}



template <typename K>
void HashIndex<K>::remove(const K& key) {

    size_t index = getHash(key, capacity);
    Node* current = buckets[index];
    Node* prev = nullptr;



    while (current != nullptr) {

        if (current->key == key) {

            if (prev == nullptr) buckets[index] = current->next;

            else prev->next = current->next;
    
            delete current;
            size--;
            return;

        }

        prev = current;
        current = current->next;

    }

}



template <typename K>
size_t HashIndex<K>::getSize() const { return size; }



template <typename K>
size_t HashIndex<K>::getCapacity() const { return capacity; }



#endif // HASH_INDEX_HPP