#include <iostream>
#include <string>
#include "include/core/Table.hpp"      
#include "include/core/Row.hpp"        
#include "include/data_structures/LinkedList.hpp" 

using namespace std;

int main() {
    cout << "=== Veritabani Motoru Baslatiliyor (LinkedList Modu) ===" << endl;

    
    LinkedList<string> colNames;
    colNames.push_back("ID");
    colNames.push_back("Isim");
    colNames.push_back("Yas");
    colNames.push_back("Maas");

    
    LinkedList<string> colTypes;
    colTypes.push_back("INT");
    colTypes.push_back("STRING");
    colTypes.push_back("INT");
    colTypes.push_back("DOUBLE");

    
    Table* myTable = new Table("Calisanlar", colNames, colTypes);

    
    Row* row1 = new Row(1);
    row1->addCell(101);             
    row1->addCell("Ahmet Yilmaz");  
    row1->addCell(30);              
    row1->addCell(5000.50);         
    myTable->insertRow(row1);

    
    Row* row2 = new Row(2);
    row2->addCell(102);
    row2->addCell("Ayse Demir");
    row2->addCell(25);
    row2->addCell(7500.25);
    myTable->insertRow(row2);

    
    Row* row3 = new Row(3);
    row3->addCell(103);
    row3->addCell("Mehmet Ozturk");
    row3->addCell(40);
    row3->addCell(9200.00);
    myTable->insertRow(row3);

    
    myTable->print();

   
    delete myTable; 

    cout << "=== Program Guvenle Kapatildi (Memory Leak Yok) ===" << endl;

    return 0;
}