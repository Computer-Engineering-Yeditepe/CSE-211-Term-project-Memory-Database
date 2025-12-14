#include "../../include/indexes/HashIndex.hpp"
#include <iostream>

HashIndex::HashIndex(int size) : tableSize(size) {
    buckets = new IndexNode*[tableSize](); // Array'i sıfırla (NULL)
}

HashIndex::~HashIndex() {
    for (int i = 0; i < tableSize; ++i) {
        IndexNode* curr = buckets[i];
        while (curr != nullptr) {
            IndexNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
    delete[] buckets;
}

size_t HashIndex::hashFunction(const std::string& key) {
    size_t hash = 5381;
    for (char c : key) hash = ((hash << 5) + hash) + c;
    return hash % tableSize;
}

void HashIndex::insert(const Cell& key, Row* rowPtr) {
    std::string sKey;
    
    // SENİN TİPLERİN: INT, DOUBLE, STRING
    if (key.getType() == CellType::INT) {
        sKey = std::to_string(key.getInt());
    } 
    else if (key.getType() == CellType::DOUBLE) {
        sKey = std::to_string(key.getDouble());
    } 
    else if (key.getType() == CellType::STRING) {
        sKey = key.getString();
    } 
    else {
        return; // Tanımsız tip
    }

    size_t idx = hashFunction(sKey);
    
    // Head'e ekleme (Insert Front)
    IndexNode* newNode = new IndexNode;
    newNode->key = sKey;
    newNode->rowPtr = rowPtr;
    newNode->next = buckets[idx];
    buckets[idx] = newNode;
}

Row* HashIndex::search(const Cell& key) {
    std::string sKey;
    
    if (key.getType() == CellType::INT) {
        sKey = std::to_string(key.getInt());
    } 
    else if (key.getType() == CellType::DOUBLE) {
        sKey = std::to_string(key.getDouble());
    } 
    else if (key.getType() == CellType::STRING) {
        sKey = key.getString();
    } 
    else {
        return nullptr;
    }

    size_t idx = hashFunction(sKey);
    IndexNode* curr = buckets[idx];

    while (curr != nullptr) {
        if (curr->key == sKey) {
            return curr->rowPtr; // Bulundu
        }
        curr = curr->next;
    }
    return nullptr; // Bulunamadı
}