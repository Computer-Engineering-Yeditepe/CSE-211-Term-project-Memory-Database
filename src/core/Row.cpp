#include "../../include/core/Row.hpp"

Row::Row(int rowId) {
    this->id = rowId;
}

Row::~Row() {

    for (auto cell : cells) {
        delete cell;
    }
}

void Row::addCell(int value) {
    cells.push_back(new Cell(value));
}

void Row::addCell(double value) {
    cells.push_back(new Cell(value));
}

void Row::addCell(std::string value) {
    cells.push_back(new Cell(value));
}

Cell* Row::getCell(size_t index) {
    
    size_t counter = 0;
    for (auto cell : cells) {
        if (counter == index) {
            return cell;
        }
        counter++;
    }
    throw std::out_of_range("Row::getCell - Index out of bounds");
}

int Row::getId() const {
    return this->id;
}