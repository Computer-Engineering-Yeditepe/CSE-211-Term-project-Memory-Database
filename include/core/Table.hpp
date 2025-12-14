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

    void print();
    // Hash Join
    void hashJoin(Table& otherTable, int myColIndex, int otherColIndex);
};

#endif