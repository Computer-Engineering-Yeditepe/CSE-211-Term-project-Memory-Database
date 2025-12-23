#include "include/core/Cell.hpp"
#include "include/core/Row.hpp"
#include "include/indexes/HashIndex.hpp"
#include <iostream>

int main() {
    std::cout << "=== HASH INDEX TEST (INT, DOUBLE, STRING) ===\n";

    // 1. Index Oluştur
    HashIndex index;

    // 2. Satırları Oluştur (Heap'te)
    Row* r1 = new Row(101);
    r1->addCell(1);           // ID (INT)
    r1->addCell("Ahmet");     // İsim (STRING)
    r1->addCell(50.5);        // Puan (DOUBLE)

    Row* r2 = new Row(102);
    r2->addCell(2);
    r2->addCell("Ayse");
    r2->addCell(80.9);

    Row* r3 = new Row(103);
    r3->addCell(3);
    r3->addCell("Mehmet");
    r3->addCell(100.0);

    // 3. İndekse Ekle (İsimlere Göre - STRING Testi)
    std::cout << "[INFO] Veriler 'Isim' (String) uzerinden indeksleniyor...\n";
    
    // Row'dan ilgili hücreyi alıp anahtar olarak veriyoruz
    // getCell(1) -> İsim kolonu
    if (r1->getCell(1)) index.insert(*(r1->getCell(1)), r1);
    if (r2->getCell(1)) index.insert(*(r2->getCell(1)), r2);
    if (r3->getCell(1)) index.insert(*(r3->getCell(1)), r3);

    // 4. Arama Yap (Başarılı Durum)
    std::cout << "\n[TEST 1] 'Ayse' araniyor...\n";
    Row* result = index.search(Cell("Ayse"));
    
    if (result) {
        std::cout << " -> BULUNDU! Row ID: " << result->getId() << " (Beklenen: 102)\n";
    } else {
        std::cout << " -> HATA: Bulunamadi!\n";
    }

    // 5. Arama Yap (Double Testi - Ekstra)
    std::cout << "\n[TEST 2] DOUBLE Indeksleme Denemesi (Manuel)...\n";
    HashIndex doubleIndex;
    doubleIndex.insert(Cell(50.5), r1);
    
    Row* resDouble = doubleIndex.search(Cell(50.5));
    if (resDouble && resDouble->getId() == 101) {
        std::cout << " -> DOUBLE Arama Basarili! (50.5 bulundu)\n";
    } else {
        std::cout << " -> DOUBLE Arama Basarisiz.\n";
    }

    // 6. Temizlik
    delete r1;
    delete r2;
    delete r3;

    std::cout << "\n=== TEST BITTI ===\n";
    return 0;
}