#include "../../include/core/Cell.hpp"

Cell::Cell(int value) {
    this->type = CellType::INT;
    this->intValue = value;
}

Cell::Cell(double value) {
    this->type = CellType::DOUBLE;
    this->doubleValue = value;
}

Cell::Cell(std::string value) {
    this->type = CellType::STRING;
    this->stringValue = value;
}

CellType Cell::getType() const {
    return this->type;
}

int Cell::getInt() const {
    if (this->type != CellType::INT) throw std::runtime_error("Type mismatch: Not INT");
    return this->intValue;
}

double Cell::getDouble() const {
    if (this->type != CellType::DOUBLE) throw std::runtime_error("Type mismatch: Not DOUBLE");
    return this->doubleValue;
}

std::string Cell::getString() const {
    if (this->type != CellType::STRING) throw std::runtime_error("Type mismatch: Not STRING");
    return this->stringValue;
}