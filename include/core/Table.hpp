#ifndef TABLE_HPP
#define TABLE_HPP

#include <string>
#include <iostream>
#include "Row.hpp"
#include "../data_structures/LinkedList.hpp" // Senin verdiğin Iterator'lı LinkedList

class Table {
private:
    std::string name;
    
    // ÖNEMLİ: Row içinde pointerlar olduğu için burada Row* tutuyoruz.
    // Row nesnelerini kopyalamak tehlikeli olabilir.
    LinkedList<Row*> rows; 

public:
    Table(std::string tableName);
    ~Table(); // Row*'ları temizlemek için destructor şart

    void insert(Row* row); // Pointer alır
    LinkedList<Row*>& getRows();
    std::string getName() const;

    void insertRow(Row* row);
    
    
    void print() const;
    
    size_t getRowCount() const;

    std::string getName() const;
    const LinkedList<std::string>& getColumns() const; 
    const LinkedList<std::string>& getTypes() const;
    const LinkedList<Row*>& getRows() const;
};

#endif