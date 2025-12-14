#include "include/core/Table.hpp"
#include "include/core/Row.hpp"
#include "include/core/Cell.hpp"
#include <iostream>

int main() {
    std::cout << "=== HASH JOIN TESTI (INT & STRING) ===\n\n";

    // 1. Departmanlar Tablosunu Kur
    // Kolonlar: [0] DeptID (INT), [1] DeptName (STRING)
    Table depts("Departments");

    Row* d1 = new Row(101); d1->addCell(101); d1->addCell("Insan Kaynaklari");
    Row* d2 = new Row(102); d2->addCell(102); d2->addCell("Bilgi Islem (IT)");
    Row* d3 = new Row(103); d3->addCell(103); d3->addCell("Satis"); // Bu departmanda kimse yok (Test için)
    
    depts.insert(d1);
    depts.insert(d2);
    depts.insert(d3);

    // 2. Calisanlar Tablosunu Kur
    // Kolonlar: [0] EmpID (INT), [1] Name (STRING), [2] DeptID (INT - Foreign Key)
    Table emps("Employees");

    // Ali -> IT (102)
    Row* e1 = new Row(1); e1->addCell(1); e1->addCell("Ali"); e1->addCell(102);
    
    // Ayse -> IK (101)
    Row* e2 = new Row(2); e2->addCell(2); e2->addCell("Ayse"); e2->addCell(101);
    
    // Mehmet -> IT (102) - IT departmaninda birden fazla kisi var
    Row* e3 = new Row(3); e3->addCell(3); e3->addCell("Mehmet"); e3->addCell(102);

    // Zeynep -> Bilinmeyen Dept (999) - Eşleşmemesi lazım
    Row* e4 = new Row(4); e4->addCell(4); e4->addCell("Zeynep"); e4->addCell(999);

    emps.insert(e1);
    emps.insert(e2);
    emps.insert(e3);
    emps.insert(e4);

    // 3. JOIN ISLEMI
    // Senaryo: Hangi departmanda kim calisiyor?
    // Departments.Col[0] (DeptID) <---> Employees.Col[2] (DeptID)
    
    std::cout << "[INFO] Join Baslatiliyor: Departments (Col 0) JOIN Employees (Col 2)...\n";
    depts.hashJoin(emps, 0, 2);

    std::cout << "\n=== TEST BITTI ===\n";
    return 0;
}