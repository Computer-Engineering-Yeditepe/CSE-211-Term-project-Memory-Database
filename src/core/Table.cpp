#include "../../include/core/Table.h"
#include <iomanip>

Table::Table(const std::string& tableName, const Vector<std::string>& colNames, const Vector<std::string>& colTypes){

    this->name = tableName;
    this->columns = colNames;
    this->types = colTypes;

}

Table::~Table(){

    for(size_t i = 0; i<rows.size(); i++){
        delete rows[i];
    }

}

void Table::insertRow(Row* row){
    rows.push_back(row);
}

size_t Table::getRowCount() const{
    return rows.size();
}

void Table::print() const {
    std::cout << "=== TABLE: " << name << " ===" << std::endl;

    for (size_t i = 0; i < columns.size(); i++) {
        std::cout << std::left << std::setw(15) << columns[i]; 
    }
    std::cout << std::endl;
    
    for (size_t i = 0; i < columns.size() * 15; i++) std::cout << "-";
    std::cout << std::endl;

    for (size_t i = 0; i < rows.size(); i++) {
        Row* currentRow = rows[i];

        for (size_t j = 0; j < columns.size(); j++) {
            Cell* cell = currentRow->getCell(j);
            
            if (cell->getType() == CellType::INT) {
                std::cout << std::left << std::setw(15) << cell->getInt();
            } 
            else if (cell->getType() == CellType::DOUBLE) {
                std::cout << std::left << std::setw(15) << cell->getDouble();
            } 
            else if (cell->getType() == CellType::STRING) {
                std::cout << std::left << std::setw(15) << cell->getString();
            }
        }
        std::cout << std::endl;
    }
    for (size_t i = 0; i < columns.size() * 15; i++) std::cout << "=";
    std::cout << std::endl;
}