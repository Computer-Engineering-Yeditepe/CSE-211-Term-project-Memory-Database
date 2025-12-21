#ifndef TABLE_HPP
#define TABLE_HPP

#include <string>
#include <iostream>
#include "../data_structures/LinkedList.hpp"
#include "../index/HashIndex.hpp"
#include "../index/BPlusTree.hpp"
#include "Row.hpp"
class Table {
private:
    std::string name;
    
    
    LinkedList<std::string> columns; 
    LinkedList<std::string> types;
    
    
    LinkedList<Row*> rows;
    HashIndex<int> primaryIndex;
    index::BPlusTree* bTreeIndex;

public:
    
    Table(const std::string& tableName, const LinkedList<std::string>& colNames, const LinkedList<std::string>& colTypes);

    ~Table();

    void insertRow(Row* row);
    
    Row* getRowById(int id);

    void removeRow(int id);
    
    void print() const;

    index::BPlusTree* getBTree() { return bTreeIndex; }
    
    size_t getRowCount() const;

    std::string getName() const;
    const LinkedList<std::string>& getColumns() const; 
    const LinkedList<std::string>& getTypes() const;
    const LinkedList<Row*>& getRows() const;
};

#endif