#ifndef MAP_HPP
#define MAP_HPP

#include "LinkedList.hpp"
#include <string>

template <typename K, typename V>
class Map {
private:
    struct KeyValuePair {
        K key;
        V value;
        
        KeyValuePair(const K& k, const V& v) : key(k), value(v) {}
    };
    
    LinkedList<KeyValuePair> entries;
    
public:
    Map() {}
    
    ~Map() {
        clear();
    }
    
    void clear() {
        entries.clear();
    }
    
    void insert(const K& key, const V& value) {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if ((*it).key == key) {
                (*it).value = value;
                return;
            }
        }
        entries.push_back(KeyValuePair(key, value));
    }
    
    V* find(const K& key) {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if ((*it).key == key) {
                return &((*it).value);
            }
        }
        return nullptr;
    }
    
    const V* find(const K& key) const {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if ((*it).key == key) {
                return &((*it).value);
            }
        }
        return nullptr;
    }
    
    V& operator[](const K& key) {
        V* found = find(key);
        if (found == nullptr) {
            insert(key, V());
            found = find(key);
        }
        return *found;
    }
    
    size_t size() const {
        return entries.size();
    }
    
    bool empty() const {
        return entries.empty();
    }
    
    class Iterator {
    private:
        typename LinkedList<KeyValuePair>::Iterator list_it;
        
    public:
        Iterator(typename LinkedList<KeyValuePair>::Iterator it) : list_it(it) {}
        
        struct KeyValue {
            K key;
            V value;
        };
        
        KeyValue operator*() {
            KeyValuePair& pair = *list_it;
            return {pair.key, pair.value};
        }
        
        Iterator& operator++() {
            ++list_it;
            return *this;
        }
        
        bool operator==(const Iterator& other) const {
            return list_it == other.list_it;
        }
        
        bool operator!=(const Iterator& other) const {
            return list_it != other.list_it;
        }
    };
    
    Iterator begin() {
        return Iterator(entries.begin());
    }
    
    Iterator end() {
        return Iterator(entries.end());
    }
};

#endif

