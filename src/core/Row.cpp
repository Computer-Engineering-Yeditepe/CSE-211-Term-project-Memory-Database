#include "../../include/core/Row.h"


Row::Row(int rowId){
    this->id = rowId;
}

Row::~Row(){

    for(size_t i = 0 ; i < cells.size() ; i++){
        delete cells[i];
    }


}


void Row::addCell(int value){
    Cell* newCell = new Cell(value);

    cells.push_back(newCell);
}

void Row::addCell(double value){
    Cell* newCell = new Cell(value);

    cells.push_back(newCell);
}

void Row::addCell(std::string value){
    Cell* newCell = new Cell(value);

    cells.push_back(newCell);
}


Cell* Row::getCell(size_t index){

    
    return cells[index];

}

int Row::getId() const {
    return this->id;
}