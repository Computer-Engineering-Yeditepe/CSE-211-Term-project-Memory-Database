#ifndef HASHINDEX_HPP
#define HASHINDEX_HPP

#include "../core/Row.hpp"
#include "../core/Cell.hpp"
#include <string>

struct IndexNode {
    std::string key;
    Row* rowPtr;     // Row pointer tutuyoruz
    IndexNode* next;
};

class HashIndex {
private:
    IndexNode** buckets;
    int tableSize;

    // İç kullanım için hash fonksiyonu
    size_t hashFunction(const std::string& key);

public:
    HashIndex(int size = 1009);
    ~HashIndex();

    // Cell nesnesini anahtar olarak alır, Row pointer'ını saklar
    void insert(const Cell& key, Row* rowPtr);
    
    // Anahtara göre satırı bulur
    Row* search(const Cell& key);
};

#endif