#ifndef ROW_H
#define ROW_H

#include "../data_structures/Vector.h"
#include "Cell.h"
#include <string>

class Row{

private:
    Vector<Cell*> cells;
    int id;
public:

    Row(int rowId);

    ~Row();

    void addCell(int value);

    void addCell(std::string value);

    void addCell(double value);

    Cell* getCell(size_t index);

    int getId() const;
};





#endif