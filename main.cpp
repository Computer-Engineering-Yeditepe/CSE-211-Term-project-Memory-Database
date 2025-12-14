#include <iostream>
#include <string>
#include "include/core/Table.hpp"      
#include "include/core/Row.hpp"        
#include "include/data_structures/LinkedList.hpp" 
#include "include/core/Cell.hpp"

using namespace std;

int main() {
    cout << "=== Veritabani Motoru Baslatiliyor (LinkedList Modu) ===" << endl;

    // 1. Tabloyu Kur (Sadece isimle)
    // Şu anki Table yapımız kolon isimlerini parametre olarak almıyor,
    // sadece satırları saklıyor.
    Table* myTable = new Table("Calisanlar");

    // 2. Verileri Ekle
    // Row 1
    Row* row1 = new Row(1);
    row1->addCell(101);             // ID
    row1->addCell("Ahmet Yilmaz");  // Isim
    row1->addCell(30);              // Yas
    row1->addCell(5000.50);         // Maas
    myTable->insert(row1);          // insertRow degil, insert kullaniyoruz

    // Row 2
    Row* row2 = new Row(2);
    row2->addCell(102);
    row2->addCell("Ayse Demir");
    row2->addCell(25);
    row2->addCell(7500.25);
    myTable->insert(row2);

    // Row 3
    Row* row3 = new Row(3);
    row3->addCell(103);
    row3->addCell("Mehmet Ozturk");
    row3->addCell(40);
    row3->addCell(9200.00);
    myTable->insert(row3);

    // 3. Tabloyu Yazdir (Hash Join'de eklediğimiz print mantığı burada çalışır)
    cout << "\n--- Tablo Icerigi ---\n";
    LinkedList<Row*>& rows = myTable->getRows();
    
    // Iterator ile gezerek yazdiriyoruz
    for(auto it = rows.begin(); it != rows.end(); ++it) {
        (*it)->print();
    }

    // 4. Temizlik
    delete myTable; // Table destructor'ı satırları da temizler

    cout << "\n=== Program Guvenle Kapatildi (Memory Leak Yok) ===" << endl;

    return 0;
}