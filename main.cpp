#include <iostream>
#include <string>
#include "include/core/Table.h"
#include "include/core/Row.h"
#include "include/data_structures/Vector.h"

using namespace std;

int main() {
    cout << "=== Veritabani Baslatiliyor ===" << endl;

    // 1. Sütun İsimlerini ve Tiplerini Hazırla
    Vector<string> colNames;
    colNames.push_back("ID");
    colNames.push_back("Isim");
    colNames.push_back("Yas");
    colNames.push_back("Maas");

    Vector<string> colTypes;
    colTypes.push_back("INT");
    colTypes.push_back("STRING");
    colTypes.push_back("INT");
    colTypes.push_back("DOUBLE");

    // 2. Tabloyu Oluştur (Heap üzerinde)
    // "Calisanlar" adında bir tablo
    Table* myTable = new Table("Calisanlar", colNames, colTypes);

    // 3. Satır 1: Ahmet
    Row* row1 = new Row(1);
    row1->addCell(101);             // ID
    row1->addCell("Ahmet Yilmaz");  // Isim
    row1->addCell(30);              // Yas
    row1->addCell(5000.50);         // Maas
    myTable->insertRow(row1);

    // 4. Satır 2: Ayşe
    Row* row2 = new Row(2);
    row2->addCell(102);
    row2->addCell("Ayse Demir");
    row2->addCell(25);
    row2->addCell(7500.25);
    myTable->insertRow(row2);

    // 5. Satır 3: Mehmet
    Row* row3 = new Row(3);
    row3->addCell(103);
    row3->addCell("Mehmet Ozturk");
    row3->addCell(40);
    row3->addCell(9200.00);
    myTable->insertRow(row3);

    // 6. Tabloyu Ekrana Çiz
    myTable->print();

    // 7. BÜYÜK TEMİZLİK TESTİ
    // delete myTable dediğimizde:
    // -> Table Destructor çalışır -> Row'ları siler.
    // -> Row Destructor çalışır -> Cell'leri siler.
    // -> Cell zaten stringleri temizler.
    // HİÇBİR HAFIZA SIZINTISI OLMAMALI!
    delete myTable; 

    cout << "=== Program Guvenle Kapatildi ===" << endl;

    return 0;
}