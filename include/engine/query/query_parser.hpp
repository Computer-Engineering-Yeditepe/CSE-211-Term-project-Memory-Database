/**
 * @file query_parser.hpp
 * @author Öykü Aksungur
 * @brief SQL query parser
 * @date 2025
 */

#ifndef QUERY_PARSER_HPP
#define QUERY_PARSER_HPP

#include "query_types.hpp"
#include <string>

/**
 * @brief Parse a SQL query string into a Query structure
 * @param query_string SQL query string (e.g., "SELECT * FROM table WHERE id = 1")
 * @return Pointer to parsed Query structure, or nullptr on failure
 * 
 * @pre query_string != nullptr
 * @post Returns valid Query* or nullptr if parsing fails
 */
Query* query_parse(const std::string& query_string);

/**
 * @brief Destroy a Query structure and free all memory
 * @param query Query to destroy
 */
void query_destroy(Query* query);

/**
 * @brief Print a Query structure for debugging
 * @param query Query to print
 */
void query_print(const Query* query);

#endif // QUERY_PARSER_HPP

