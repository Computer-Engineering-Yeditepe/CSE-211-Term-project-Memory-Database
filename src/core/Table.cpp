#include "../../include/core/Table.hpp"
#include <iomanip> // std::setw için

Table::Table(const std::string& tableName, const LinkedList<std::string>& colNames, const LinkedList<std::string>& colTypes) {
    this->name = tableName;
    
    for(const auto& col : colNames) this->columns.push_back(col);
    for(const auto& type : colTypes) this->types.push_back(type);
}

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

    primaryIndex.insert(row->getId(), row);
}

Row* Table::getRowById(int id) { return primaryIndex.search(id);}

void Table::removeRow(int id) {
    // Silinecek satırı önce bulalım
    Row* rowToDelete = primaryIndex.search(id);
    
    if (rowToDelete != nullptr) {
        primaryIndex.remove(id);

        rows.remove(rowToDelete); 

        delete rowToDelete;
        
        std::cout << "ID: " << id << " silindi." << std::endl;
    } else {
        std::cout << "HATA: Silinecek ID (" << id << ") bulunamadi." << std::endl;
    }
}

LinkedList<Row*>& Table::getRows() {
    return rows;
}

std::string Table::getName() const {
    return name;
}

void Table::print() {
    std::cout << "--- TABLO: " << name << " ---\n";
    // Iterator ile satırları gezip yazdırıyoruz
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        (*it)->print();
    }
    std::cout << "-----------------------\n";
}

// --- HASH JOIN ALGORİTMASI ---
void Table::hashJoin(Table& otherTable, int myColIndex, int otherColIndex) {
    std::cout << "--- HASH JOIN: " << this->name << " JOIN " << otherTable.getName() << " ---\n";

    int bucketSize = 1009;
    JoinNode** hashTable = new JoinNode*[bucketSize](); // Pointer dizisi

    // 1. FAZ: BUILD (İnşa Etme)
    // ITERATOR KULLANIMI: Senin LinkedList yapına uygun şekilde begin() ve end() kullanıyoruz.
    for (auto it = this->rows.begin(); it != this->rows.end(); ++it) {
        Row* currentRow = *it; // Iterator'dan Row pointer'ını al
        
        // Satırdan hücreyi al (Cell*)
        Cell* cell = currentRow->getCell(myColIndex);
        if (!cell) continue; // Hücre yoksa atla

        // Anahtarı string'e çevir
        std::string key;
        if (cell->getType() == CellType::INT) key = std::to_string(cell->getInt());
        else if (cell->getType() == CellType::STRING) key = cell->getString();
        else key = "NULL"; 

        size_t idx = customHash(key, bucketSize);
        
        // Hash tablosuna ekle
        JoinNode* newNode = new JoinNode;
        newNode->key = key;
        newNode->rowPtr = currentRow;
        newNode->next = hashTable[idx];
        hashTable[idx] = newNode;
    }

    // 2. FAZ: PROBE (Sorgulama)
    // Diğer tablonun iteratörleri ile geziyoruz
    int matchCount = 0;
    LinkedList<Row*>& otherRows = otherTable.getRows();

    std::cout << "Matches:\n";
    for (auto it = otherRows.begin(); it != otherRows.end(); ++it) {
        Row* otherRow = *it;
        
        Cell* cell = otherRow->getCell(otherColIndex);
        if (!cell) continue;

        std::string key;
        if (cell->getType() == CellType::INT) key = std::to_string(cell->getInt());
        else if (cell->getType() == CellType::STRING) key = cell->getString();
        else key = "NULL";

        size_t idx = customHash(key, bucketSize);

        // Kovadaki (Bucket) zinciri gez
        JoinNode* cursor = hashTable[idx];
        while (cursor != nullptr) {
            if (cursor->key == key) {
                // EŞLEŞME BULUNDU
                std::cout << " >> ID:" << cursor->rowPtr->getId() 
                          << " <-> ID:" << otherRow->getId() << "\n";
                matchCount++;
            }
            cursor = cursor->next;
        }
    }

    std::cout << "=== Total Matches: " << matchCount << " ===\n\n";

    // CLEANUP (Bellek Temizliği)
    for (int i = 0; i < bucketSize; ++i) {
        JoinNode* curr = hashTable[i];
        while (curr != nullptr) {
            JoinNode* toDelete = curr;
            curr = curr->next;
            delete toDelete;
        }
    }
    std::cout << "====================================" << std::endl;
}

std::string Table::getName() const {
    return this->name;
}

const LinkedList<std::string>& Table::getColumns() const {
    return this->columns;
}

const LinkedList<std::string>& Table::getTypes() const {
    return this->types;
}

const LinkedList<Row*>& Table::getRows() const {
    return this->rows;
}