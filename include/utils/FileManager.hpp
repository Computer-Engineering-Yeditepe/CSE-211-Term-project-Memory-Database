#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <string>
#include <fstream>
#include <iostream>


#include "../../libs/json.hpp" 

#include "../core/Table.hpp"


using json = nlohmann::json;

class FileManager {
public:
    
    static void saveTable(const Table* table, const std::string& filename);

    static Table* loadTable(const std::string& filename);
};

#endif