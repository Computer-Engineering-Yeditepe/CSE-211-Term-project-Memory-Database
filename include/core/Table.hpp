#ifndef TABLE_HPP
#define TABLE_HPP

#include <string>
#include <iostream>
#include "Row.hpp"
#include "../data_structures/LinkedList.hpp"
#include "../indexes/HashIndex.hpp"
#include "../index/BPlusTree.hpp"

class Table {
private:
    std::string name;
    LinkedList<std::string> columns;
    LinkedList<std::string> types;
    LinkedList<Row*> rows;
    HashIndex primaryIndex;
    db_index::BPlusTree* bTreeIndex;

public:
    Table(std::string tableName);
    ~Table();

    void insert(Row* row);
    LinkedList<Row*>& getRows();
    std::string getName() const;
    void insertRow(Row* row);
    void print() const;
    db_index::BPlusTree* getBTree() { return bTreeIndex; }
    size_t getRowCount() const;
    const LinkedList<std::string>& getColumns() const;
    const LinkedList<std::string>& getTypes() const;
    const LinkedList<Row*>& getRows() const;
};

#endif
