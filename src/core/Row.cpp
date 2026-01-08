#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp"
#include <iostream>

Row::Row(int rowId) : id(rowId) {}

Row::~Row() {
    for(auto it = cells.begin(); it != cells.end(); ++it) {
        delete *it; 
    }
}

void Row::addCell(int value) { cells.push_back(new Cell(value)); }
void Row::addCell(double value) { cells.push_back(new Cell(value)); }
void Row::addCell(std::string value) { cells.push_back(new Cell(value)); }

Cell* Row::getCell(size_t index) {
    size_t count = 0;
    for(auto it = cells.begin(); it != cells.end(); ++it) {
        if (count == index) return *it;
        count++;
    }
    return nullptr;
}

int Row::getId() const { return id; }

// --- ARTIK ÇOK DAHA KISA VE TEMİZ ---
void Row::print() {
    std::cout << "[ID:" << id << "] ";
    for(auto it = cells.begin(); it != cells.end(); ++it) {
        if (*it) {
            // Cell sınıfındaki "operator<<" sayesinde direkt yazdırıyoruz.
            // if-else bloklarına gerek kalmadı!
            std::cout << *(*it) << " | "; 
        }
    }
    std::cout << "\n";
}