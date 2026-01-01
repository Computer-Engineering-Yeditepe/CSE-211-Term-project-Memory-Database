#include "../../include/core/Table.hpp"
#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp" 
#include <iostream>
#include <string>

Table::Table(const std::string& tableName, const LinkedList<std::string>& colNames, const LinkedList<std::string>& colTypes)
    : primaryIndex(16) {
    this->name = tableName;
    
    this->bTreeIndex = new db_index::BPlusTree(4);

    for(const auto& col : colNames) this->columns.push_back(col);
    for(const auto& type : colTypes) this->types.push_back(type);
}

Table::Table(std::string tableName) : name(tableName) {
    this->bTreeIndex = new db_index::BPlusTree(4);
}

Table::~Table() {
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        delete *it;
    }
    delete bTreeIndex;
}

void Table::insert(Row* row) {
    rows.push_back(row);
    primaryIndex.insert(row->getId(), row);
    index::RecordID record = {0, 0, row};
    bTreeIndex->insert(row->getId(), record);
}

Row* Table::getRowById(int id) { 
    return primaryIndex.search(id);
}

void Table::removeRow(int id) {
    Row* rowToDelete = primaryIndex.search(id);
    
    if (rowToDelete != nullptr) {
        primaryIndex.remove(id);
        rows.remove(rowToDelete); 
        bTreeIndex->remove(id);
        delete rowToDelete;
        std::cout << "ID: " << id << " silindi." << std::endl;
    } else {
        std::cout << "HATA: Silinecek ID (" << id << ") bulunamadi." << std::endl;
    }
}

void Table::insertRow(Row* row) {
    rows.push_back(row);
}

LinkedList<Row*>& Table::getRows() {
    return rows;
}

std::string Table::getName() const {
    return name;
}

void Table::print() const {
    std::cout << "--- TABLO: " << name << " ---\n";
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        (*it)->print();
    }
    std::cout << "-----------------------\n";
}

size_t Table::getRowCount() const {
    size_t count = 0;
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        count++;
    }
    return count;
}

const LinkedList<std::string>& Table::getColumns() const {
    return columns;
}

const LinkedList<std::string>& Table::getTypes() const {
    return types;
}

const LinkedList<Row*>& Table::getRows() const {
    return rows;
}
