#ifndef TABLE_HPP
#define TABLE_HPP

#include <string>
#include <iostream>
#include "../data_structures/LinkedList.hpp"
#include "Row.hpp"

class Table {
private:
    std::string name;
    
    
    LinkedList<std::string> columns; 
    LinkedList<std::string> types;
    
    
    LinkedList<Row*> rows;

public:
    
    Table(const std::string& tableName, const LinkedList<std::string>& colNames, const LinkedList<std::string>& colTypes);

    ~Table();

    void insertRow(Row* row);
    
    
    void print() const;
    
    size_t getRowCount() const;
};

#endif