#include "../../include/utils/FileManager.hpp"
#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp"
#include <fstream>
#include <iostream>

void FileManager::saveTable(const Table* table, const std::string& filename) {
    json j; 

    j["table_name"] = table->getName();

    j["columns"] = json::array();
    for (const auto& col : table->getColumns()) {
        j["columns"].push_back(col);
    }

    j["column_types"] = json::array();
    for (const auto& type : table->getTypes()) {
        j["column_types"].push_back(type);
    }

    j["rows"] = json::array();

    for (const auto& row : table->getRows()) {
        json rowJson = json::array(); 
        
        
        for (const auto& cell : row->getCells()) {
            
            if (cell->getType() == CellType::INT) {
                rowJson.push_back(cell->getInt());
            } 
            else if (cell->getType() == CellType::DOUBLE) {
                rowJson.push_back(cell->getDouble());
            } 
            else if (cell->getType() == CellType::STRING) {
                rowJson.push_back(cell->getString());
            }
        }
        
        j["rows"].push_back(rowJson);
    }

    
    std::string fullPath = "data/" + filename;
    
    std::ofstream file(fullPath);
    if (file.is_open()) {
        
        file << j.dump(4); 
        file.close();
        std::cout << "Basarili: Tablo '" << fullPath << "' dosyasina kaydedildi." << std::endl;
    } else {
        std::cerr << "Hata: Dosya acilamadi -> " << fullPath << std::endl;
    }
}

Table* FileManager::loadTable(const std::string& filename) {
    std::string fullPath = "data/" + filename;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        std::cerr << "Hata: Dosya okunamadi -> " << fullPath << std::endl;
        return nullptr;
    }

    json j;
    try {
        file >> j; 
    } catch (const json::parse_error& e) {
        std::cerr << "Hata: JSON parse hatasi: " << e.what() << std::endl;
        file.close();
        return nullptr;
    }
    file.close();

    
    std::string tableName = j["table_name"];

    
    LinkedList<std::string> columns;
    LinkedList<std::string> types;

    for (const auto& col : j["columns"]) {
        columns.push_back(col.get<std::string>());
    }

    for (const auto& type : j["column_types"]) {
        types.push_back(type.get<std::string>());
    }

    
    Table* newTable = new Table(tableName, columns, types);

    
    
    
    for (const auto& rowJson : j["rows"]) {
        
        Row* newRow = new Row(0); 
        
        
        
        auto typeIt = types.begin();

        for (const auto& cellValue : rowJson) {
            
            if (typeIt == types.end()) break;

            std::string type = *typeIt; 

            if (type == "INT") {
                newRow->addCell(cellValue.get<int>());
            } 
            else if (type == "DOUBLE") {
                newRow->addCell(cellValue.get<double>());
            } 
            else if (type == "STRING") {
                newRow->addCell(cellValue.get<std::string>());
            }
            
            ++typeIt; 
        }
        
        
        newTable->insertRow(newRow);
    }

    std::cout << "Basarili: Tablo '" << tableName << "' dosyadan yuklendi (" << newTable->getRowCount() << " satir)." << std::endl;
    return newTable;
}