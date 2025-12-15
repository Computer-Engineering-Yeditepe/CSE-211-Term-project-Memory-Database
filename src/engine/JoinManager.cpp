#include "../../include/engine/JoinManager.hpp"
#include "../../include/indexes/HashIndex.hpp"
#include <iostream>

Table* JoinManager::hashJoin(Table* t1, int col1Index, Table* t2, int col2Index) {
    // 1. Sonuç için yeni bir tablo oluştur
    std::string newName = t1->getName() + "_JOIN_" + t2->getName();
    Table* resultTable = new Table(newName);

    // ====================================================
    // 1. AŞAMA: BUILD PHASE (İNŞA ETME)
    // t1 tablosunu senin HashIndex yapına yüklüyoruz.
    // ====================================================
    
    // Hash tablosunun boyutunu t1'in satır sayısına göre ayarlayabilirsin 
    // ama varsayılan 1009 da yeterli.
    HashIndex tempIndex(1009);

    LinkedList<Row*>& rows1 = t1->getRows();
    
    // t1 satırlarını gez
    for (auto it = rows1.begin(); it != rows1.end(); ++it) {
        Row* row = *it;
        
        // Birleşecek sütundaki hücreyi al (Key)
        Cell* keyCell = row->getCell(col1Index);
        
        if (keyCell != nullptr) {
            // Senin HashIndex'ine ekle: Anahtar -> Satır Pointer'ı
            tempIndex.insert(*keyCell, row);
        }
    }

    // ====================================================
    // 2. AŞAMA: PROBE PHASE (TARAMA/EŞLEŞTİRME)
    // t2 tablosunu satır satır gezip HashIndex'te arıyoruz.
    // ====================================================

    LinkedList<Row*>& rows2 = t2->getRows();

    for (auto it = rows2.begin(); it != rows2.end(); ++it) {
        Row* row2 = *it; // t2'den gelen satır
        
        // t2'nin birleşme anahtarını al
        Cell* keyCell2 = row2->getCell(col2Index);

        if (keyCell2 != nullptr) {
            // HashIndex'te bu anahtarı ara
            Row* row1 = tempIndex.search(*keyCell2);

            if (row1 != nullptr) {
                // --- EŞLEŞME BULUNDU (MATCH) ---
                
                // Yeni bir sonuç satırı oluştur (ID'si sırayla artan)
                // resultTable->getRowCount() henüz public değilse rastgele id verebilirsin
                Row* newRow = new Row(0); 

                // A. Önce Tablo 1'in hücrelerini kopyala
                LinkedList<Cell*>& cells1 = row1->getCells();
                for (auto c = cells1.begin(); c != cells1.end(); ++c) {
                    Cell* original = *c;
                    // Tipe göre yeni Cell oluştur (Deep Copy)
                    if (original->getType() == CellType::INT) 
                        newRow->addCell(original->getInt());
                    else if (original->getType() == CellType::DOUBLE) 
                        newRow->addCell(original->getDouble());
                    else 
                        newRow->addCell(original->getString());
                }

                // B. Sonra Tablo 2'nin hücrelerini kopyala
                LinkedList<Cell*>& cells2 = row2->getCells();
                for (auto c = cells2.begin(); c != cells2.end(); ++c) {
                    Cell* original = *c;
                    if (original->getType() == CellType::INT) 
                        newRow->addCell(original->getInt());
                    else if (original->getType() == CellType::DOUBLE) 
                        newRow->addCell(original->getDouble());
                    else 
                        newRow->addCell(original->getString());
                }

                // Birleşmiş satırı sonuç tablosuna ekle
                resultTable->insert(newRow);
            }
        }
    }

    // tempIndex burada scope dışına çıkınca destructor'ı çalışır 
    // ve hash tablosunu temizler. (Row* silinmez, sadece index node'lar silinir)
    
    return resultTable;
}