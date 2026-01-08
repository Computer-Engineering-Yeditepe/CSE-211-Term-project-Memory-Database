#include <iostream>
#include <string>
#include "include/core/Table.hpp"
#include "include/core/Row.hpp"
#include "include/data_structures/LinkedList.hpp"
#include "include/utils/FileManager.hpp"
#include "libs/httplib.h" // httplib kütüphanesini ekledik
 
using namespace std;
 
int main() {
    // 1. Veritabanını Dosyadan Yükle (Veya sıfırdan oluştur)
    Table* dbTable = FileManager::loadTable("test_db.json");
    if (dbTable == nullptr) {
        cout << "Dosya bulunamadi, bos tablo olusturuluyor..." << endl;
        LinkedList<string> colNames;
        colNames.push_back("ID"); colNames.push_back("Isim"); colNames.push_back("Yas");
        LinkedList<string> colTypes;
        colTypes.push_back("INT"); colTypes.push_back("STRING"); colTypes.push_back("INT");
        dbTable = new Table("Personel", colNames, colTypes);
    }
 
    // 2. HTTP Sunucusunu Başlat
    httplib::Server svr;
 
    // API: ID ile veri getir (Örn: localhost:8080/get?id=1)
    svr.Get("/get", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("id")) {
            int id = stoi(req.get_param_value("id"));
            Row* row = dbTable->getRowById(id); //
            if (row) {
                // Basit bir yanıt formatı (İleride JSON'a çevrilebilir)
                res.set_content("ID: " + to_string(row->getId()) + " bulundu.", "text/plain");
            } else {
                res.status = 404;
                res.set_content("Kayit bulunamadi", "text/plain");
            }
        }
    });
 
    // API: Veritabanını Kaydet (Örn: localhost:8080/save)
    svr.Get("/save", [&](const httplib::Request& req, httplib::Response& res) {
        FileManager::saveTable(dbTable, "test_db.json"); //
        res.set_content("Veritabani dosyaya kaydedildi.", "text/plain");
    });
 
    cout << "Sunucu 8080 portunda baslatildi..." << endl;
    cout << "Durdurmak icin Ctrl+C tusuna basin." << endl;
    svr.listen("0.0.0.0", 8080);
 
    // Program kapanırken temizlik yap
    delete dbTable;
    return 0;
}
