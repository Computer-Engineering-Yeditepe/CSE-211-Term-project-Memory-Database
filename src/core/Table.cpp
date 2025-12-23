#include "../../include/core/Table.hpp"
#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp" 
#include <iostream>
#include <string>


// --- YARDIMCI YAPILAR ---

struct JoinNode {
    std::string key;
    Row* rowPtr;
    JoinNode* next;
};

// Basit Hash Fonksiyonu
size_t customHash(const std::string& key, int tableSize) {
    size_t hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % tableSize;
}

// --- TABLE IMPLEMENTASYONU ---


Table::Table(std::string tableName) : name(tableName) {}

Table::~Table() {
    // Tablo silinirken içindeki Row pointerlarını da temizlemeliyiz
    // LinkedList iteratörü ile gezip siliyoruz
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        delete *it; // *it bize Row* verir
    }
    // LinkedList'in kendi destructor'ı node'ları temizler, biz içeriği temizledik.
}

void Table::insert(Row* row) {
    rows.push_back(row);
}

LinkedList<Row*>& Table::getRows() {
    return rows;
}

std::string Table::getName() const {
    return name;
}

void Table::print() const {
    std::cout << "--- TABLO: " << name << " ---\n";
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        (*it)->print();
    }
    std::cout << "-----------------------\n";
}

void Table::insertRow(Row* row) {
    rows.push_back(row);
}

size_t Table::getRowCount() const {
    size_t count = 0;
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        count++;
    }
    return count;
}

const LinkedList<std::string>& Table::getColumns() const {
    return columns;
}

const LinkedList<std::string>& Table::getTypes() const {
    return types;
}

const LinkedList<Row*>& Table::getRows() const {
    return rows;
}