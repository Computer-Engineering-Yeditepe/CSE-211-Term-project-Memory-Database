#ifndef JOIN_ENGINE_HPP
#define JOIN_ENGINE_HPP

#include "../../core/Table.hpp"
#include "query_types.hpp"
#include <string>

/**
 * @brief Nested Loop Join algoritması ile iki tabloyu birleştirir
 * 
 * Bu fonksiyon, sol tablodaki her satırı sağ tablodaki tüm satırlarla karşılaştırarak
 * eşleşen satırları birleştirir. Küçük tablolar için uygundur.
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
                        JoinType join_type);

/**
 * @brief Hash Join algoritması ile iki tabloyu birleştirir
 * 
 * Bu fonksiyon, sol tabloyu hash tablosuna yerleştirip sağ tablodaki satırları
 * hash tablosunda arar. Büyük tablolar için nested loop'dan daha verimlidir.
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
                 JoinType join_type);

/**
 * @brief Join condition'a göre iki tabloyu birleştirir
 * 
 * Bu fonksiyon, join condition'dan sütun isimlerini alıp index'lerini bulur,
 * tablo boyutlarına göre nested loop veya hash join algoritmasını seçer.
 * 
 * @param left_table Sol tablo (left table)
 * @param right_table Sağ tablo (right table)
 * @param condition Join condition (hangi sütunlar üzerinden join yapılacak)
 * @return Birleştirilmiş yeni tablo pointer'ı, hata durumunda nullptr
 */
Table* join_execute(Table* left_table, Table* right_table,
                    const JoinCondition& condition);

#endif
