#ifndef ROW_HPP
#define ROW_HPP

#include <string>
#include "../data_structures/LinkedList.hpp" 
#include "Cell.hpp"

class Row {
private:
    int id;
    LinkedList<Cell*> cells; 

public:
    Row(int rowId);
    ~Row();

    void addCell(int value);
    void addCell(double value);
    void addCell(std::string value);

    Cell* getCell(size_t index);
    int getId() const;

    
    LinkedList<Cell*>& getCells() { return cells; }
};

#endif