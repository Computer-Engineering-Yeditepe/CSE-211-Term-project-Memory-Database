#ifndef QUERY_PARSER_HPP
#define QUERY_PARSER_HPP

#include "query_types.hpp"
#include <string>

/**
 * @brief SQL benzeri query string'ini parse eder ve Query yapısına dönüştürür
 * 
 * Bu fonksiyon, SELECT, FROM, WHERE, JOIN, ORDER BY, LIMIT gibi SQL clause'larını
 * parse eder ve Query yapısını doldurur.
 * 
 * @param query_string Parse edilecek SQL query string'i
 * @return Parse edilmiş Query pointer'ı, hata durumunda nullptr
 */
Query* query_parse(const std::string& query_string);

/**
 * @brief Query yapısını ve içindeki tüm verileri bellekten siler
 * 
 * Bu fonksiyon, dinamik olarak oluşturulmuş Query yapısını temizler.
 * 
 * @param query Silinecek Query pointer'ı
 */
void query_destroy(Query* query);

/**
 * @brief Query yapısını ekrana yazdırır (debug için)
 * 
 * Bu fonksiyon, Query yapısının içeriğini okunabilir formatta ekrana basar.
 * 
 * @param query Yazdırılacak Query pointer'ı
 */
void query_print(const Query* query);

#endif
