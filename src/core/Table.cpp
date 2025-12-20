#include "../../include/core/Table.hpp"
#include <iomanip> // std::setw için

Table::Table(const std::string& tableName, const LinkedList<std::string>& colNames, const LinkedList<std::string>& colTypes)
    : primaryIndex(16) {
    this->name = tableName;
    
    for(const auto& col : colNames) this->columns.push_back(col);
    for(const auto& type : colTypes) this->types.push_back(type);
}

Table::~Table() {
    for (auto row : rows) {
        delete row;
    }
}

void Table::insertRow(Row* row) {
    rows.push_back(row);

    primaryIndex.insert(row->getId(), row);
}

Row* Table::getRowById(int id) { return primaryIndex.search(id);}

void Table::removeRow(int id) {
    // Silinecek satırı önce bulalım
    Row* rowToDelete = primaryIndex.search(id);
    
    if (rowToDelete != nullptr) {
        primaryIndex.remove(id);

        rows.remove(rowToDelete); 

        delete rowToDelete;
        
        std::cout << "ID: " << id << " silindi." << std::endl;
    } else {
        std::cout << "HATA: Silinecek ID (" << id << ") bulunamadi." << std::endl;
    }
}

size_t Table::getRowCount() const {
    return rows.size();
}

void Table::print() const {
    std::cout << "=== TABLE: " << name << " ===" << std::endl;

    
    for (const auto& col : columns) {
        std::cout << std::left << std::setw(15) << col;
    }
    std::cout << std::endl;

    
    for (size_t i = 0; i < columns.size() * 15; i++) std::cout << "-";
    std::cout << std::endl;

    
    for (const auto& row : rows) {
        
        LinkedList<Cell*>& cells = row->getCells();
        for (auto cell : cells) {
            if (cell->getType() == CellType::INT) {
                std::cout << std::left << std::setw(15) << cell->getInt();
            } else if (cell->getType() == CellType::DOUBLE) {
                std::cout << std::left << std::setw(15) << cell->getDouble();
            } else if (cell->getType() == CellType::STRING) {
                std::cout << std::left << std::setw(15) << cell->getString();
            }
        }
        std::cout << std::endl;
    }
    std::cout << "====================================" << std::endl;
}

std::string Table::getName() const {
    return this->name;
}

const LinkedList<std::string>& Table::getColumns() const {
    return this->columns;
}

const LinkedList<std::string>& Table::getTypes() const {
    return this->types;
}

const LinkedList<Row*>& Table::getRows() const {
    return this->rows;
}