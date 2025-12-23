#include <iostream>
#include <string>
#include "include/core/Table.hpp"
#include "include/core/Row.hpp"
#include "include/data_structures/LinkedList.hpp"
#include "include/utils/FileManager.hpp" // Dosya yöneticisi

using namespace std;

int main() {
    cout << "=== 1. VERITABANI OLUSTURULUYOR ===" << endl;

    // Sütun İsimleri
    LinkedList<string> colNames;
    colNames.push_back("ID");
    colNames.push_back("Isim");
    colNames.push_back("Yas");

    // Sütun Tipleri
    LinkedList<string> colTypes;
    colTypes.push_back("INT");
    colTypes.push_back("STRING");
    colTypes.push_back("INT");

    // Tabloyu Kur
    Table* originalTable = new Table("Personel", colNames, colTypes);

    // Satır 1
    Row* r1 = new Row(1); 
    r1->addCell(1); 
    r1->addCell("Ali Veli"); 
    r1->addCell(25);
    originalTable->insertRow(r1);

    // Satır 2
    Row* r2 = new Row(2); 
    r2->addCell(2); 
    r2->addCell("Zeynep Kaya"); 
    r2->addCell(30);
    originalTable->insertRow(r2);

    cout << "Orijinal Tablo:" << endl;
    originalTable->print();

    // ---------------------------------------------------------
    
    cout << "\n=== 2. DOSYAYA KAYDEDILIYOR ===" << endl;
    // data/test_db.json dosyasına yazacak
    FileManager::saveTable(originalTable, "test_db.json");

    // ---------------------------------------------------------

    cout << "\n=== 3. HAFIZA TEMIZLENIYOR (Tablo Siliniyor) ===" << endl;
    delete originalTable; // Veriler RAM'den silindi!
    
    // ---------------------------------------------------------

    cout << "\n=== 4. DOSYADAN GERI YUKLENIYOR ===" << endl;
    // Dosyadan okuyup SIFIRDAN yeni bir tablo yaratıyoruz
    Table* loadedTable = FileManager::loadTable("test_db.json");

    if (loadedTable != nullptr) {
        cout << "Yuklenen Tablo:" << endl;
        loadedTable->print();
        
        // İşimiz bitince bunu da silmeliyiz
        delete loadedTable;
    } else {
        cout << "HATA: Tablo yuklenemedi!" << endl;
    }

    return 0;
}