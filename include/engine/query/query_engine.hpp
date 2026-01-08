#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include "../../core/Table.hpp"
#include "query_types.hpp"
#include "../../data_structures/LinkedList.hpp"
#include "../../data_structures/Map.hpp"
#include <string>

/**
 * @brief Veritabanı sınıfı - tabloları yönetir
 * 
 * Bu sınıf, veritabanındaki tüm tabloları Map yapısında tutar.
 * Tablo ekleme, getirme ve isim listesi alma işlemlerini sağlar.
 */
class Database {
private:
    Map<std::string, Table*> tables;

public:
    /**
     * @brief Veritabanı constructor'ı
     */
    Database();
    
    /**
     * @brief Veritabanı destructor'ı
     */
    ~Database();
    
    /**
     * @brief Veritabanına yeni bir tablo ekler
     * @param table Eklenecek tablo pointer'ı
     * @return Başarılı ise 0, hata durumunda -1
     */
    int addTable(Table* table);
    
    /**
     * @brief İsme göre tablo getirir
     * @param table_name Aranacak tablo ismi
     * @return Bulunan tablo pointer'ı, bulunamazsa nullptr
     */
    Table* getTable(const std::string& table_name);
    
    /**
     * @brief Veritabanındaki tüm tablo isimlerini döndürür
     * @return Tablo isimlerinin listesi
     */
    LinkedList<std::string> getTableNames() const;
};

/**
 * @brief SQL benzeri query'yi çalıştırır
 * 
 * Bu fonksiyon, Query yapısındaki tüm işlemleri sırayla uygular:
 * 1. FROM (tablo seçimi)
 * 2. JOIN (tablo birleştirme)
 * 3. WHERE (filtreleme, index optimizasyonu ile)
 * 4. SELECT (sütun seçimi)
 * 5. ORDER BY (sıralama)
 * 6. LIMIT (sayfalama)
 * 
 * @param db Veritabanı pointer'ı
 * @param query Çalıştırılacak query yapısı
 * @return Sonuç tablosu pointer'ı, hata durumunda nullptr
 */
Table* query_execute(Database* db, const Query* query);

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
Table* query_apply_where(Table* table, const LinkedList<QueryCondition>& conditions);

/**
 * @brief SELECT sütunlarını tabloya uygular (projection)
 * 
 * Bu fonksiyon, sadece belirtilen sütunları içeren yeni bir tablo oluşturur.
 * 
 * @param table Kaynak tablo
 * @param column_names Seçilecek sütun isimleri listesi
 * @return Sadece seçilen sütunları içeren yeni tablo pointer'ı
 */
Table* query_apply_select(Table* table, const LinkedList<std::string>& column_names);

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
Table* query_apply_order_by(Table* table, const LinkedList<std::string>& column_names, bool ascending);

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
Table* query_apply_limit(Table* table, int limit, int offset);

#endif
