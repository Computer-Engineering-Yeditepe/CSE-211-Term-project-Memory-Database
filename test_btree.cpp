#include "include/core/Row.hpp"
#include "include/core/Cell.hpp"
#include "include/indexes/BPlusTree.hpp"
#include <iostream>

int main() {
    std::cout << "\n========================================\n";
    std::cout << "   B+ TREE TESTI (YAPRAK GEZINTISI)   \n";
    std::cout << "========================================\n";

    // Dereceyi 3 seçiyoruz ki çabuk bölünsün (Split test edilsin)
    BPlusTree tree(3);

    // Test Verileri (Heap Allocation)
    Row* r1 = new Row(10); r1->addCell(10);
    Row* r2 = new Row(20); r2->addCell(20);
    Row* r3 = new Row(5);  r3->addCell(5);
    Row* r4 = new Row(6);  r4->addCell(6);
    Row* r5 = new Row(12); r5->addCell(12);
    Row* r6 = new Row(30); r6->addCell(30);
    Row* r7 = new Row(15); r7->addCell(15);

    // 1. ADIM: KARIŞIK EKLEME
    // Sıralı vermiyoruz ki ağaç kendisi sıralasın.
    std::cout << "[INFO] Veriler ekleniyor: 10, 20, 5, 6, 12, 30, 15\n";
    
    tree.insert(Cell(10), r1);
    tree.insert(Cell(20), r2);
    tree.insert(Cell(5), r3);  // En başa gitmeli
    tree.insert(Cell(6), r4);  // Araya gitmeli
    tree.insert(Cell(12), r5); // Araya gitmeli
    tree.insert(Cell(30), r6); // En sona gitmeli
    tree.insert(Cell(15), r7); // Araya gitmeli

    // 2. ADIM: YAZDIRMA (KRİTİK TEST)
    // Yeni print() fonksiyonu en soldaki yapraktan başlayıp sağa doğru gider.
    // Eğer burası SIRALI çıkarsa, B+ Tree %100 çalışıyor demektir.
    std::cout << "\n[SONUC] Agactaki Veriler (Sirali Olmali):\n";
    std::cout << "----------------------------------------\n";
    std::cout << "CIKTI:    ";
    tree.print(); 
    std::cout << "\nBEKLENEN: 5 6 10 12 15 20 30\n";
    std::cout << "----------------------------------------\n";

    // 3. ADIM: ARAMA TESTİ
    std::cout << "\n[TEST] 12 sayisi araniyor...\n";
    Row* found = tree.search(Cell(12));
    if (found) {
        std::cout << " -> BASARILI: ID 12 bulundu.\n";
    } else {
        std::cout << " -> HATA: 12 bulunamadi!\n";
    }

    std::cout << "\n[TEST] 99 sayisi araniyor (Olmayan)...\n";
    Row* notFound = tree.search(Cell(99));
    if (!notFound) {
        std::cout << " -> BASARILI: Olmayan veri bulunamadi.\n";
    } else {
        std::cout << " -> HATA: Olmayan veri bulundu!\n";
    }

    std::cout << "\n=== TEST BITTI ===\n";
    return 0;
}