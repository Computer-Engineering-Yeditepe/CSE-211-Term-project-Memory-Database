#ifndef CELL_HPP
#define CELL_HPP

#include <string>
#include <iostream>

enum class CellType {
    INT,
    DOUBLE,
    STRING
};

class Cell {
private:
    CellType type;
    
    // Verileri tutan alanlar
    // Union bellek tasarrufu sağlar ama string sığmaz, onu ayrı tutuyoruz.
    union {
        int intValue;
        double doubleValue;
    };
    std::string stringValue;

public:
    // --- CONSTRUCTORS (KURUCULAR) ---
    Cell(); // Default Constructor (B+ Tree dizileri için ŞART!)
    Cell(int v);
    Cell(double v);
    Cell(std::string v);

    // --- GETTERS (VERİ ALMA) ---
    CellType getType() const;
    int getInt() const;
    double getDouble() const;
    std::string getString() const;

    // --- OPERATORS (B+ TREE VE SORT İÇİN ŞART) ---
    // Küçüktür (<) operatörü: Sıralama için
    bool operator<(const Cell& other) const;
    // Büyüktür (>) operatörü
    bool operator>(const Cell& other) const;
    // Eşittir (==) operatörü
    bool operator==(const Cell& other) const;
    // Eşit Değildir (!=) operatörü
    bool operator!=(const Cell& other) const;

    // --- PRINT OPERATÖRÜ (Ekrana yazdırmak için) ---
    friend std::ostream& operator<<(std::ostream& os, const Cell& c);
};

#endif