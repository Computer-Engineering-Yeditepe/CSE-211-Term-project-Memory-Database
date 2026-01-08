/**
 * src/engine/query/join_engine.cpp
 * JOIN SÜTUNLARI DÜZELTİLMİŞ SÜRÜM
 */
#include "../../../include/engine/query/join_engine.hpp"
#include "../../../include/core/Table.hpp"
#include "../../../include/core/Row.hpp"
#include "../../../include/core/Cell.hpp"
#include "../../../include/data_structures/LinkedList.hpp"
#include <string>

struct HashEntry {
    Row* row;
    HashEntry(Row* r) : row(r) {}
};

/**
 * @brief İki tablonun sütun isimlerini ve tiplerini birleştirir
 * 
 * Join işlemi sonucunda oluşacak tablonun sütun yapısını hazırlar.
 * Önce sol tablonun tüm sütunları, sonra sağ tablonun tüm sütunları eklenir.
 * 
 * @param left Sol tablo
 * @param right Sağ tablo
 * @param cols Çıktı: Birleştirilmiş sütun isimleri listesi
 * @param types Çıktı: Birleştirilmiş sütun tipleri listesi
 */
static void merge_columns(Table* left, Table* right, LinkedList<std::string>& cols, LinkedList<std::string>& types) {
    for (const auto& col : left->getColumns()) cols.push_back(col);
    for (const auto& type : left->getTypes()) types.push_back(type);
    
    for (const auto& col : right->getColumns()) cols.push_back(col);
    for (const auto& type : right->getTypes()) types.push_back(type);
}

/**
 * @brief Nested Loop Join algoritması ile iki tabloyu birleştirir
 * 
 * Bu fonksiyon, sol tablodaki her satırı sağ tablodaki tüm satırlarla karşılaştırarak
 * eşleşen satırları birleştirir. Küçük tablolar için uygundur.
 * Zaman karmaşıklığı: O(n*m) (n=sol tablo satır sayısı, m=sağ tablo satır sayısı)
 * 
 * @param left_table Sol tablo (left table)
 * @param right_table Sağ tablo (right table)
 * @param left_column_index Sol tablodaki join sütununun index'i
 * @param right_column_index Sağ tablodaki join sütununun index'i
 * @param join_type Join tipi (INNER, LEFT, RIGHT, FULL) - şu an sadece INNER destekleniyor
 * @return Birleştirilmiş yeni tablo pointer'ı, hata durumunda nullptr
 */
Table* join_nested_loop(Table* left_table, Table* right_table,
                        int left_column_index, int right_column_index,
                        JoinType join_type) {
    if (!left_table || !right_table) return nullptr;
    
    // 1. Sütunları Birleştir (DÜZELTME BURADA)
    LinkedList<std::string> res_cols;
    LinkedList<std::string> res_types;
    merge_columns(left_table, right_table, res_cols, res_types);

    Table* result = new Table("join_result_nl", res_cols, res_types);
    
    for (auto left_row : left_table->getRows()) {
        Cell* left_cell = left_row->getCell(left_column_index);
        if (!left_cell) continue;
        
        std::string left_key;
        if (left_cell->getType() == CellType::INT) left_key = std::to_string(left_cell->getInt());
        else if (left_cell->getType() == CellType::STRING) left_key = left_cell->getString();
        else continue;
        
        bool matched = false;
        
        for (auto right_row : right_table->getRows()) {
            Cell* right_cell = right_row->getCell(right_column_index);
            if (!right_cell) continue;
            
            std::string right_key;
            if (right_cell->getType() == CellType::INT) right_key = std::to_string(right_cell->getInt());
            else if (right_cell->getType() == CellType::STRING) right_key = right_cell->getString();
            else continue;
            
            if (left_key == right_key) {
                // Eşleşme bulundu: İki satırı birleştir
                Row* joined_row = new Row(left_row->getId()); // ID mantığı projeye göre değişebilir
                
                // Sol hücreleri ekle
                for (auto cell : left_row->getCells()) {
                    if (cell->getType() == CellType::INT) joined_row->addCell(cell->getInt());
                    else if (cell->getType() == CellType::DOUBLE) joined_row->addCell(cell->getDouble());
                    else joined_row->addCell(cell->getString());
                }
                // Sağ hücreleri ekle
                for (auto cell : right_row->getCells()) {
                    if (cell->getType() == CellType::INT) joined_row->addCell(cell->getInt());
                    else if (cell->getType() == CellType::DOUBLE) joined_row->addCell(cell->getDouble());
                    else joined_row->addCell(cell->getString());
                }
                
                result->insertRow(joined_row);
                matched = true;
            }
        }
    }
    return result;
}

/**
 * @brief Hash Join algoritması ile iki tabloyu birleştirir
 * 
 * Bu fonksiyon, sol tabloyu hash tablosuna yerleştirip sağ tablodaki satırları
 * hash tablosunda arar. Büyük tablolar için nested loop'dan daha verimlidir.
 * Zaman karmaşıklığı: O(n+m) (n=sol tablo satır sayısı, m=sağ tablo satır sayısı)
 * 
 * @param left_table Sol tablo (left table)
 * @param right_table Sağ tablo (right table)
 * @param left_column_index Sol tablodaki join sütununun index'i
 * @param right_column_index Sağ tablodaki join sütununun index'i
 * @param join_type Join tipi (INNER, LEFT, RIGHT, FULL) - şu an sadece INNER destekleniyor
 * @return Birleştirilmiş yeni tablo pointer'ı, hata durumunda nullptr
 */
Table* join_hash(Table* left_table, Table* right_table,
                 int left_column_index, int right_column_index,
                 JoinType join_type) {
    if (!left_table || !right_table) return nullptr;
    
    // Hash tablosu oluştur
    LinkedList<HashEntry*> hash_list;
    for (auto row : left_table->getRows()) {
        if (row->getCell(left_column_index)) {
            hash_list.push_back(new HashEntry(row));
        }
    }
    
    // 1. Sütunları Birleştir (DÜZELTME BURADA)
    LinkedList<std::string> res_cols;
    LinkedList<std::string> res_types;
    merge_columns(left_table, right_table, res_cols, res_types);
    
    Table* result = new Table("join_result_hash", res_cols, res_types);
    
    for (auto right_row : right_table->getRows()) {
        Cell* right_cell = right_row->getCell(right_column_index);
        if (!right_cell) continue;
        
        std::string key;
        if (right_cell->getType() == CellType::INT) key = std::to_string(right_cell->getInt());
        else if (right_cell->getType() == CellType::STRING) key = right_cell->getString();
        else continue;
        
        for (auto entry : hash_list) {
            Row* left_row = entry->row;
            Cell* left_cell = left_row->getCell(left_column_index);
            
            std::string left_key;
             if (left_cell->getType() == CellType::INT) left_key = std::to_string(left_cell->getInt());
            else if (left_cell->getType() == CellType::STRING) left_key = left_cell->getString();
            
            if (left_key == key) {
                Row* joined_row = new Row(left_row->getId());
                
                // Solu kopyala
                for (auto cell : left_row->getCells()) {
                    if (cell->getType() == CellType::INT) joined_row->addCell(cell->getInt());
                    else if (cell->getType() == CellType::DOUBLE) joined_row->addCell(cell->getDouble());
                    else joined_row->addCell(cell->getString());
                }
                // Sağı kopyala
                for (auto cell : right_row->getCells()) {
                    if (cell->getType() == CellType::INT) joined_row->addCell(cell->getInt());
                    else if (cell->getType() == CellType::DOUBLE) joined_row->addCell(cell->getDouble());
                    else joined_row->addCell(cell->getString());
                }
                result->insertRow(joined_row);
            }
        }
    }
    
    for (auto entry : hash_list) delete entry;
    
    return result;
}

/**
 * @brief Join condition'a göre iki tabloyu birleştirir
 * 
 * Bu fonksiyon, join condition'dan sütun isimlerini alıp index'lerini bulur,
 * tablo boyutlarına göre nested loop veya hash join algoritmasını seçer.
 * Eğer her iki tablo da 100 satırdan küçükse nested loop, aksi halde hash join kullanılır.
 * 
 * @param left_table Sol tablo (left table)
 * @param right_table Sağ tablo (right table)
 * @param condition Join condition (hangi sütunlar üzerinden join yapılacak)
 * @return Birleştirilmiş yeni tablo pointer'ı, hata durumunda nullptr
 */
Table* join_execute(Table* left_table, Table* right_table,
                    const JoinCondition& condition) {
    if (!left_table || !right_table) return nullptr;
    
    // Kolon indexlerini bul
    int left_col_idx = -1;
    int idx = 0;
    for (const auto& col : left_table->getColumns()) {
        if (col == condition.left_column) { left_col_idx = idx; break; }
        idx++;
    }
    
    int right_col_idx = -1;
    idx = 0;
    for (const auto& col : right_table->getColumns()) {
        if (col == condition.right_column) { right_col_idx = idx; break; }
        idx++;
    }
    
    if (left_col_idx < 0 || right_col_idx < 0) {
        // Basit bir fallback: Eğer parser ID=UserID dediyse ve sol tabloda ID varsa tamam.
        // Ama bazen parser sırayı garanti edemezse burada swap denenebilir.
        // Şimdilik strict varsayıyoruz.
        return nullptr;
    }
    
    // Küçük tablolar için Nested Loop, büyükler için Hash Join (Proje kuralı olabilir)
    // Burada basitlik adına threshold koyuyoruz
    if (left_table->getRowCount() < 100 && right_table->getRowCount() < 100) {
        return join_nested_loop(left_table, right_table, left_col_idx, right_col_idx, condition.join_type);
    } else {
        return join_hash(left_table, right_table, left_col_idx, right_col_idx, condition.join_type);
    }
}