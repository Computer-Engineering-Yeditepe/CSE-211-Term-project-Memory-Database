/**
 * src/engine/query/query_engine.cpp
 * DÜZELTİLMİŞ NİHAİ SÜRÜM (JOIN BLOĞU EKLENDİ)
 */
#include "../../../include/engine/query/query_engine.hpp"
#include "../../../include/engine/query/join_engine.hpp"
#include "../../../include/core/Table.hpp"
#include "../../../include/core/Row.hpp"
#include "../../../include/core/Cell.hpp"
#include <algorithm>
#include <iostream>
#include <string>

/**
 * @brief Veritabanı constructor'ı
 * Boş bir veritabanı oluşturur.
 */
Database::Database() {}

/**
 * @brief Veritabanı destructor'ı
 * Tabloların silinmesi için kullanılır (şu an boş).
 */
Database::~Database() {}

/**
 * @brief Veritabanına yeni bir tablo ekler
 * @param table Eklenecek tablo pointer'ı
 * @return Başarılı ise 0, hata durumunda -1
 */
int Database::addTable(Table* table) {
    if (!table) return -1;
    tables.insert(table->getName(), table);
    return 0;
}

/**
 * @brief İsme göre tablo getirir
 * @param table_name Aranacak tablo ismi
 * @return Bulunan tablo pointer'ı, bulunamazsa nullptr
 */
Table* Database::getTable(const std::string& table_name) {
    Table** found = tables.find(table_name);
    return found ? *found : nullptr;
}

/**
 * @brief Veritabanındaki tüm tablo isimlerini döndürür
 * @return Tablo isimlerinin listesi
 */
LinkedList<std::string> Database::getTableNames() const {
    LinkedList<std::string> names;
    for (auto it = tables.begin(); it != tables.end(); ++it) names.push_back((*it).key);
    return names;
}

/**
 * @brief Sütun ismine göre sütun index'ini bulur
 * 
 * Bu yardımcı fonksiyon, tablodaki sütun isimlerini tarayarak
 * belirtilen sütunun index'ini döndürür.
 * 
 * @param table Aranacak tablo
 * @param column_name Aranacak sütun ismi
 * @return Sütun index'i, bulunamazsa -1
 */
static int get_column_index_by_name(Table* table, const std::string& column_name) {
    if (!table) return -1;
    int index = 0;
    for (const auto& col : table->getColumns()) {
        if (col == column_name) return index;
        index++;
    }
    return -1;
}

/**
 * @brief Bir satırın WHERE koşulunu karşılayıp karşılamadığını kontrol eder
 * 
 * Bu fonksiyon, sayısal ve string karşılaştırmaları destekler.
 * Desteklenen operatörler: EQUAL, NOT_EQUAL, LESS_THAN, LESS_EQUAL,
 * GREATER_THAN, GREATER_EQUAL, LIKE
 * 
 * @param row Kontrol edilecek satır
 * @param condition WHERE koşulu
 * @param table Satırın ait olduğu tablo (sütun bilgisi için)
 * @return Koşul sağlanıyorsa true, aksi halde false
 */
static bool evaluate_condition(Row* row, const QueryCondition& condition, Table* table) {
    if (!row || !table) return false;
    
    int col_idx = get_column_index_by_name(table, condition.column_name);
    if (col_idx < 0) return false;
    
    Cell* cell = row->getCell(col_idx);
    if (!cell) return false;
    
    std::string cell_val_str;
    if (cell->getType() == CellType::INT) cell_val_str = std::to_string(cell->getInt());
    else if (cell->getType() == CellType::DOUBLE) cell_val_str = std::to_string(cell->getDouble());
    else cell_val_str = cell->getString();
    
    bool is_numeric = (cell->getType() == CellType::INT || cell->getType() == CellType::DOUBLE);
    double cell_num = 0, cond_num = 0;
    
    if (is_numeric) {
        try {
            cell_num = std::stod(cell_val_str);
            cond_num = std::stod(condition.value);
        } catch (...) { is_numeric = false; }
    }

    switch (condition.op) {
        case ComparisonOperator::EQUAL: return cell_val_str == condition.value;
        case ComparisonOperator::NOT_EQUAL: return cell_val_str != condition.value;
        case ComparisonOperator::LESS_THAN: return is_numeric ? (cell_num < cond_num) : (cell_val_str < condition.value);
        case ComparisonOperator::LESS_EQUAL: return is_numeric ? (cell_num <= cond_num) : (cell_val_str <= condition.value);
        case ComparisonOperator::GREATER_THAN: return is_numeric ? (cell_num > cond_num) : (cell_val_str > condition.value);
        case ComparisonOperator::GREATER_EQUAL: return is_numeric ? (cell_num >= cond_num) : (cell_val_str >= condition.value);
        case ComparisonOperator::LIKE: return cell_val_str.find(condition.value) != std::string::npos;
        default: return false;
    }
}

/**
 * @brief WHERE koşullarını tabloya uygular
 * 
 * Bu fonksiyon, verilen koşullara göre tablodaki satırları filtreler.
 * Sayısal ve string karşılaştırmaları destekler.
 * 
 * @param table Filtrelenecek tablo
 * @param conditions WHERE koşulları listesi
 * @return Filtrelenmiş yeni tablo pointer'ı, hata durumunda orijinal tablo
 */
Table* query_apply_where(Table* table, const LinkedList<QueryCondition>& conditions) {
    if (!table || conditions.empty()) return table;
    
    Table* result = new Table(table->getName() + "_filtered", table->getColumns(), table->getTypes());
    
    for (auto row : table->getRows()) {
        bool matches = true;
        for (const auto& cond : conditions) {
            if (!evaluate_condition(row, cond, table)) {
                matches = false;
                break;
            }
        }
        
        if (matches) {
            Row* new_row = new Row(row->getId());
            for (auto cell : row->getCells()) {
                if (cell->getType() == CellType::INT) new_row->addCell(cell->getInt());
                else if (cell->getType() == CellType::DOUBLE) new_row->addCell(cell->getDouble());
                else new_row->addCell(cell->getString());
            }
            result->insertRow(new_row);
        }
    }
    return result;
}

/**
 * @brief SELECT sütunlarını tabloya uygular (projection)
 * 
 * Bu fonksiyon, sadece belirtilen sütunları içeren yeni bir tablo oluşturur.
 * SQL'deki SELECT sütun seçimi işlemini gerçekleştirir.
 * 
 * @param table Kaynak tablo
 * @param column_names Seçilecek sütun isimleri listesi
 * @return Sadece seçilen sütunları içeren yeni tablo pointer'ı
 */
Table* query_apply_select(Table* table, const LinkedList<std::string>& column_names) {
    if (!table || column_names.empty()) return table;

    LinkedList<int> col_indices;
    LinkedList<std::string> new_col_names;
    LinkedList<std::string> new_col_types;

    const auto& all_cols = table->getColumns();
    const auto& all_types = table->getTypes();

    for (const auto& target : column_names) {
        int idx = 0;
        auto type_it = all_types.begin();
        for (const auto& col : all_cols) {
            if (col == target) {
                col_indices.push_back(idx);
                new_col_names.push_back(col);
                new_col_types.push_back(*type_it);
                break;
            }
            idx++;
            ++type_it;
        }
    }

    if (col_indices.empty()) return table;

    Table* result = new Table("Projected_Result", new_col_names, new_col_types);

    for (auto row : table->getRows()) {
        Row* new_row = new Row(row->getId());
        for (int idx : col_indices) {
            Cell* old_cell = row->getCell(idx);
            if (old_cell) {
                if (old_cell->getType() == CellType::INT) new_row->addCell(old_cell->getInt());
                else if (old_cell->getType() == CellType::DOUBLE) new_row->addCell(old_cell->getDouble());
                else new_row->addCell(old_cell->getString());
            }
        }
        result->insertRow(new_row);
    }
    return result;
}

/**
 * @brief LIMIT ve OFFSET'i tabloya uygular
 * 
 * Bu fonksiyon, belirtilen sayıda satırı belirtilen offset'ten başlayarak döndürür.
 * Sayfalama (pagination) için kullanılır.
 * 
 * @param table Kaynak tablo
 * @param limit Döndürülecek maksimum satır sayısı (-1 ise tümü)
 * @param offset Atlanacak satır sayısı
 * @return Limit ve offset uygulanmış yeni tablo pointer'ı
 */
Table* query_apply_limit(Table* table, int limit, int offset) {
    if (!table) return nullptr;
    if (limit < 0 && offset == 0) return table;

    Table* result = new Table(table->getName() + "_limit", table->getColumns(), table->getTypes());
    int current = 0;
    int taken = 0;

    for (auto row : table->getRows()) {
        if (current < offset) { current++; continue; }
        if (limit >= 0 && taken >= limit) break;

        Row* new_row = new Row(row->getId());
        for (auto cell : row->getCells()) {
             if (cell->getType() == CellType::INT) new_row->addCell(cell->getInt());
             else if (cell->getType() == CellType::DOUBLE) new_row->addCell(cell->getDouble());
             else new_row->addCell(cell->getString());
        }
        result->insertRow(new_row);
        taken++;
        current++;
    }
    return result;
}

/**
 * @brief ORDER BY sıralamasını tabloya uygular
 * 
 * Bu fonksiyon, belirtilen sütunlara göre tabloyu sıralar.
 * Şu an basit bir implementasyon (pass-through).
 * 
 * @param table Sıralanacak tablo
 * @param column_names Sıralama yapılacak sütun isimleri
 * @param ascending Artan sıralama (true) veya azalan sıralama (false)
 * @return Sıralanmış tablo pointer'ı (şu an orijinal tablo döner)
 */
Table* query_apply_order_by(Table* table, const LinkedList<std::string>& column_names, bool ascending) {
    return table; // Basitlik için pass geçiyoruz
}

/**
 * @brief SQL benzeri query'yi çalıştırır
 * 
 * Bu fonksiyon, Query yapısındaki tüm işlemleri sırayla uygular:
 * 1. FROM (tablo seçimi)
 * 2. JOIN (tablo birleştirme - nested loop veya hash join)
 * 3. WHERE (filtreleme, ID sütunu için index optimizasyonu ile)
 * 4. SELECT (sütun seçimi/projection)
 * 5. ORDER BY (sıralama - şu an pass-through)
 * 6. LIMIT (sayfalama)
 * 
 * Özel optimizasyon: WHERE koşulunda "ID = değer" varsa Hash Index kullanılır (O(1) arama).
 * 
 * @param db Veritabanı pointer'ı
 * @param query Çalıştırılacak query yapısı
 * @return Sonuç tablosu pointer'ı, hata durumunda nullptr
 */
Table* query_execute(Database* db, const Query* query) {
    if (!db || !query || query->from_tables.empty()) return nullptr;
    
    std::string table_name = *(query->from_tables.begin());
    Table* current_table = db->getTable(table_name);
    if (!current_table) return nullptr;

    Table* result = current_table;
    bool is_temporary = false;

    // --- EKSİK OLAN KISIM BURASIYDI: JOIN ---
    if (!query->joins.empty()) {
        for (auto it = query->joins.begin(); it != query->joins.end(); ++it) {
            const JoinCondition& join = *it;
            Table* right_table = db->getTable(join.right_table);
            if (right_table) {
                Table* joined = join_execute(result, right_table, join);
                if (joined) {
                    if (is_temporary && result != current_table) delete result;
                    result = joined;
                    is_temporary = true;
                }
            }
        }
    }
    // ----------------------------------------

    // WHERE
    if (!query->conditions.empty()) {
        
        // --- [YENI] OPTIMIZASYON: "ID = Deger" sorgusunda Index Kullan ---
        // Eger islem henuz basindaysa (Join yapilmadiysa) ve ID araniyorsa
        if (result == current_table) { 
            for (auto it = query->conditions.begin(); it != query->conditions.end(); ++it) {
                const QueryCondition& cond = *it;
                
                // Kosul "ID" kolonunda "EQUAL" ise Hash Index'i devreye sok
                if (cond.column_name == "ID" && cond.op == ComparisonOperator::EQUAL) {
                    try {
                        int search_id = std::stoi(cond.value);
                        
                        // Index Uzerinden Hizli Arama (O(1))
                        Row* found_row = result->getRowById(search_id);
                        
                        // Sadece bulunan satiri iceren kucuk bir tablo olustur
                        Table* index_result = new Table(result->getName() + "_index_scan", result->getColumns(), result->getTypes());
                        
                        if (found_row) {
                            // Satirin kopyasini al (Deep Copy)
                            Row* new_row = new Row(found_row->getId());
                            for(auto cell : found_row->getCells()) {
                                if (cell->getType() == CellType::INT) new_row->addCell(cell->getInt());
                                else if (cell->getType() == CellType::DOUBLE) new_row->addCell(cell->getDouble());
                                else new_row->addCell(cell->getString());
                            }
                            index_result->insertRow(new_row);
                        }
                        
                        // Pointerlari guncelle
                        if (is_temporary && result != current_table) delete result;
                        result = index_result;
                        is_temporary = true;
                        
                        // Index kullandik, artik donguden cikabiliriz. 
                        // Diger kosullar (varsa) asagidaki standart filtrelemede kontrol edilecek.
                        break; 
                    } catch (...) {
                        // ID integer degilse index kullanma, normal yola devam et
                    }
                }
            }
        }

        Table* filtered = query_apply_where(result, query->conditions);
        if (filtered && filtered != result) {
            if (is_temporary && result != current_table) delete result;
            result = filtered;
            is_temporary = true;
        }
    }

    // SELECT
    if (!query->select_columns.empty()) {
        Table* projected = query_apply_select(result, query->select_columns);
        if (projected && projected != result) {
            if (is_temporary && result != current_table) delete result;
            result = projected;
            is_temporary = true;
        }
    }
    
    // LIMIT
    if (query->limit >= 0) {
        Table* limited = query_apply_limit(result, query->limit, query->offset);
        if (limited && limited != result) {
             if (is_temporary && result != current_table) delete result;
             result = limited;
             is_temporary = true;
        }
    }

    if (result == current_table) {
        LinkedList<std::string> all_cols = current_table->getColumns();
        result = query_apply_select(current_table, all_cols);
    }
    
    return result;
}