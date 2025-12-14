#include <iostream>
#include <cassert>
#include <string>
#include "../include/index/HashIndex.hpp"

// Mock Row structure
struct Row {
    int id;
    std::string data;
};

int main() {
    std::cout << "Testing Hash Index Implementation..." << std::endl;

    // Initialize
    HashIndex<int> index(10);
    Row r1 = {1, "Alice"};
    Row r2 = {2, "Bob"};
    
    // 1. Test Insert
    index.insert(1, &r1);
    index.insert(2, &r2);
    assert(index.getSize() == 2);
    std::cout << "[PASS] Insert successful." << std::endl;

    // 2. Test Search
    Row* found = index.search(1);
    assert(found != nullptr);
    assert(found->data == "Alice");
    std::cout << "[PASS] Search successful." << std::endl;

    // 3. Test Remove
    index.remove(1);
    assert(index.search(1) == nullptr);
    assert(index.getSize() == 1);
    std::cout << "[PASS] Remove successful." << std::endl;

    std::cout << "All tests passed!" << std::endl;
    return 0;
}