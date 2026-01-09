#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "include/core/Table.hpp"
#include "include/core/Row.hpp"
#include "include/core/Cell.hpp"
#include "include/data_structures/LinkedList.hpp"
#include "include/utils/FileManager.hpp"
#include "libs/httplib.h"

using namespace std;

// Global Veritabanı
Table* dbTable = nullptr;

// JSON Yardımcısı
string rowToJson(Row* row, const LinkedList<string>& colNames) {
    stringstream ss;
    ss << "{";
    ss << "\"id\": " << row->getId() << ", ";
    ss << "\"data\": {";
    LinkedList<Cell*>& cells = row->getCells();
    
    auto itCol = colNames.begin();
    auto itCell = cells.begin();
    bool first = true;
    
    while(itCol != colNames.end() && itCell != cells.end()) {
        if(!first) ss << ", ";
        ss << "\"" << *itCol << "\": ";
        Cell* cell = *itCell;
        
        if (cell->getType() == CellType::STRING) {
            ss << "\"" << cell->getString() << "\"";
        } else if (cell->getType() == CellType::INT) {
            ss << cell->getInt();
        } else if (cell->getType() == CellType::DOUBLE) {
            ss << cell->getDouble();
        }
        
        ++itCol; ++itCell; first = false;
    }
    ss << "}}";
    return ss.str();
}

int main() {
    cout << "==========================================" << endl;
    cout << "   C++ DB Engine (Debug & Universal GET)  " << endl;
    cout << "==========================================" << endl;

    // 1. Veritabanını Yükle
    dbTable = FileManager::loadTable("test_db.json");
    if (dbTable == nullptr) {
        cout << "[BILGI] Tablo bulunamadi, varsayilan 'users' tablosu olusturuluyor..." << endl;
        // Varsayılan Tablo Yapısı
        LinkedList<string> cols; cols.push_back("id"); cols.push_back("name"); cols.push_back("age");
        LinkedList<string> types; types.push_back("INT"); types.push_back("STRING"); types.push_back("INT");
        dbTable = new Table("users", cols, types);
    }

    httplib::Server svr;

    // --- 1. TÜM VERİYİ GETİR (/get_all) ---
    svr.Get("/get_all", [&](const httplib::Request&, httplib::Response& res) {
        if(!dbTable) { res.set_content("{}", "application/json"); return; }
        
        stringstream ss;
        ss << "{ \"" << dbTable->getName() << "\": {";
        
        // Sütunlar
        ss << "\"columns\": [";
        const LinkedList<string>& cols = dbTable->getColumns();
        bool first = true;
        for(auto col : cols) { if(!first) ss << ","; ss << "\"" << col << "\""; first = false; }
        ss << "],";

        // Tipler
        ss << "\"types\": [";
        const LinkedList<string>& types = dbTable->getTypes();
        first = true;
        for(auto t : types) { if(!first) ss << ","; ss << "\"" << t << "\""; first = false; }
        ss << "],";
        
        // Satırlar
        ss << "\"rows\": [";
        const LinkedList<Row*>& rows = dbTable->getRows();
        bool firstRow = true;
        for(auto row : rows) {
            if(!firstRow) ss << ",";
            ss << "{";
            LinkedList<Cell*>& cells = row->getCells();
            auto itCol = cols.begin(); auto itCell = cells.begin(); bool fc = true;
            while(itCol != cols.end() && itCell != cells.end()) {
                if(!fc) ss << ",";
                ss << "\"" << *itCol << "\": ";
                if ((*itCell)->getType() == CellType::STRING) ss << "\"" << (*itCell)->getString() << "\"";
                else if ((*itCell)->getType() == CellType::INT) ss << (*itCell)->getInt();
                else ss << (*itCell)->getDouble();
                ++itCol; ++itCell; fc = false;
            }
            ss << "}"; firstRow = false;
        }
        ss << "], \"indexes\": {\"btree\": [], \"hash\": []}}}";
        res.set_content(ss.str(), "application/json");
    });

    // --- 2. VERİ EKLE (/insert) - DEBUG MODU ---
    svr.Get("/insert", [&](const httplib::Request& req, httplib::Response& res) {
        if(!dbTable) { res.status = 400; return; }
        
        cout << "\n[DEBUG] --- Insert Istegi Basladi ---" << endl;

        // Gelen Parametreleri Logla (Hata Ayıklama İçin)
        cout << "[DEBUG] Gelen Parametreler:" << endl;
        for (const auto& param : req.params) {
            cout << "   -> Anahtar: [" << param.first << "] Deger: [" << param.second << "]" << endl;
        }

        // Yeni ID Hesapla
        int rowCount = 0;
        const LinkedList<Row*>& rows = dbTable->getRows();
        auto itRow = rows.begin();
        while(itRow != rows.end()) { rowCount++; ++itRow; }
        int newId = rowCount + 1;

        // Yeni Satırı Oluştur
        Row* newRow = new Row(newId);
        
        const LinkedList<string>& cols = dbTable->getColumns();
        const LinkedList<string>& types = dbTable->getTypes();
        LinkedList<Cell*>& rowCells = newRow->getCells();

        auto itCol = cols.begin();
        auto itType = types.begin();
        
        while(itCol != cols.end()) {
            string colName = *itCol;
            string type = (itType != types.end()) ? *itType : "STRING";
            
            string val = "";
            
            // Parametre kontrolü
            if (req.has_param(colName.c_str())) {
                val = req.get_param_value(colName.c_str());
            } else {
                cout << "[UYARI] Parametre eksik: [" << colName << "]. Bos gecilecek." << endl;
            }

            // Hücre Oluşturma ve Dönüştürme
            Cell* newCell = nullptr;
            try {
                if(type == "INT") {
                    int iVal = 0;
                    if(!val.empty()) {
                        try { iVal = stoi(val); } 
                        catch(...) { iVal = 0; cout << "[HATA] " << colName << " icin INT donusumu basarisiz: " << val << endl; }
                    }
                    newCell = new Cell(iVal);
                }
                else if(type == "DOUBLE") {
                    double dVal = 0.0;
                    if(!val.empty()) {
                        try { dVal = stod(val); } 
                        catch(...) { dVal = 0.0; cout << "[HATA] " << colName << " icin DOUBLE donusumu basarisiz: " << val << endl; }
                    }
                    newCell = new Cell(dVal);
                }
                else {
                    newCell = new Cell(val);
                }
            } catch (...) {
                newCell = new Cell("ERROR");
            }
            
            rowCells.push_back(newCell);
            
            ++itCol;
            if(itType != types.end()) ++itType;
        }

        dbTable->insertRow(newRow);
        cout << "[OK] Yeni satir eklendi. ID: " << newId << endl;
        res.set_content("{\"status\": \"inserted\"}", "application/json");
    });

    // --- 3. SATIR SİL (/delete) ---
    svr.Get("/delete", [&](const httplib::Request& req, httplib::Response& res) {
        if(req.has_param("id")) {
            try {
                int id = stoi(req.get_param_value("id"));
                dbTable->removeRow(id);
                cout << "[OK] Satir silindi. ID: " << id << endl;
                res.set_content("{\"status\": \"deleted\"}", "application/json");
            } catch(...) {
                res.status = 400;
            }
        }
    });

    // --- 4. TABLO OLUŞTUR (/create_table) ---
    svr.Get("/create_table", [&](const httplib::Request& req, httplib::Response& res) {
        string name = req.get_param_value("name");
        string colStr = req.get_param_value("columns");
        string typeStr = req.get_param_value("types");

        cout << "[INFO] Yeni Tablo Istegi: " << name << endl;

        if(dbTable) delete dbTable;

        LinkedList<string> cols;
        LinkedList<string> types;

        stringstream ssCols(colStr); string segment;
        while(getline(ssCols, segment, ',')) cols.push_back(segment);

        stringstream ssTypes(typeStr);
        while(getline(ssTypes, segment, ',')) types.push_back(segment);

        dbTable = new Table(name, cols, types);
        res.set_content("{\"status\": \"table_created\"}", "application/json");
    });
    
    // --- 5. QUERY SİMÜLASYONU (/query) ---
    svr.Get("/query", [&](const httplib::Request& req, httplib::Response& res) {
        string query = req.get_param_value("query");
        cout << "[SQL LOG] " << query << endl;
        res.set_content("{\"status\": \"success\", \"msg\": \"Sorgu alindi ve loglandi.\"}", "application/json");
    });

    cout << "Sunucu 8080 portunda dinleniyor..." << endl;
    cout << "Durdurmak icin Ctrl+C tusuna basin." << endl;
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}