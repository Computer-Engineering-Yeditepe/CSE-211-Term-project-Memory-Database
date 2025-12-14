#include "../../include/core/Cell.hpp"

// --- CONSTRUCTORS ---

// Varsayılan Kurucu: Boş bir INT (0) oluşturur.
// Bu sayede "new Cell[10]" diyebilirsin.
Cell::Cell() : type(CellType::INT), intValue(0) {}

Cell::Cell(int v) : type(CellType::INT), intValue(v) {}

Cell::Cell(double v) : type(CellType::DOUBLE), doubleValue(v) {}

Cell::Cell(std::string v) : type(CellType::STRING), stringValue(v), intValue(0) {}

// --- GETTERS ---

CellType Cell::getType() const {
    return type;
}

int Cell::getInt() const {
    return intValue;
}

double Cell::getDouble() const {
    return doubleValue;
}

std::string Cell::getString() const {
    return stringValue;
}

// --- OPERATORS (KARŞILAŞTIRMA MANTIĞI) ---

bool Cell::operator<(const Cell& other) const {
    // Tipler farklıysa tipleri kıyasla (Örn: INT < STRING)
    if (type != other.type) {
        return type < other.type;
    }

    // Tipler aynıysa değerleri kıyasla
    switch (type) {
        case CellType::INT:    return intValue < other.intValue;
        case CellType::DOUBLE: return doubleValue < other.doubleValue;
        case CellType::STRING: return stringValue < other.stringValue;
        default: return false;
    }
}

bool Cell::operator==(const Cell& other) const {
    if (type != other.type) return false;

    switch (type) {
        case CellType::INT:    return intValue == other.intValue;
        case CellType::DOUBLE: return doubleValue == other.doubleValue;
        case CellType::STRING: return stringValue == other.stringValue;
        default: return false;
    }
}

// Diğer operatörleri yukarıdakileri kullanarak tanımlıyoruz
bool Cell::operator>(const Cell& other) const {
    return !(*this < other) && !(*this == other);
}

bool Cell::operator!=(const Cell& other) const {
    return !(*this == other);
}

// --- PRINT OPERATOR (<<) ---
// Artık "cout << cell" diyebileceksin.
std::ostream& operator<<(std::ostream& os, const Cell& c) {
    switch (c.type) {
        case CellType::INT:    os << c.intValue; break;
        case CellType::DOUBLE: os << c.doubleValue; break;
        case CellType::STRING: os << c.stringValue; break;
    }
    return os;
}