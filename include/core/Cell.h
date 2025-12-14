#ifndef CELL_H
#define CELL_H

#include <string>
#include <iostream>
#include <stdexcept> 

// Veri Tipleri Etiketi
enum class CellType {
    INT,
    DOUBLE,   // Veya DOUBLE
    STRING
};

class Cell {
private:
    CellType type;      
    
    // Verileri tutacak değişkenler
    int intValue;
    double doubleValue;
    std::string stringValue;

public:
    Cell(int value);
    Cell(double value);
    Cell(std::string value);

    CellType getType() const;

    int getInt() const;
    double getDouble() const;
    std::string getString() const;
};

#endif