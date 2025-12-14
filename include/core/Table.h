#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <iostream>
#include "../data_structures/Vector.h"
#include "Row.h"


class Table{

private:

    std::string name;
    Vector<std::string> columns;
    Vector<std::string> types;
    Vector<Row*> rows;

public:
    Table(const std::string& tableName, const Vector<std::string>& colNames, const Vector<std::string>& colTypes);

    ~Table();

    size_t getRowCount() const;
    
    void insertRow(Row* row);

    void print() const;

};






#endif