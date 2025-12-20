/**
 * @file query_engine.hpp
 * @author Öykü Aksungur
 * @brief Main query execution engine
 * @date 2025
 */

#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include "../core/Table.hpp"
#include "query_types.hpp"
#include <string>
#include <vector>
#include <map>

/**
 * @class Database
 * @brief Represents a database with multiple tables
 */
class Database {
private:
    std::map<std::string, Table*> tables;  ///< Map of table name to table pointer

public:
    Database();
    ~Database();

    /**
     * @brief Add a table to the database
     * @param table Table to add
     * @return 0 on success, -1 on failure
     */
    int addTable(Table* table);

    /**
     * @brief Get a table by name
     * @param table_name Name of the table
     * @return Pointer to table, or nullptr if not found
     */
    Table* getTable(const std::string& table_name);

    /**
     * @brief Get all table names
     * @return Vector of table names
     */
    std::vector<std::string> getTableNames() const;
};

/**
 * @brief Execute a query on the database
 * @param db Target database
 * @param query Parsed query to execute
 * @return Pointer to result table, or nullptr on failure
 * 
 * @pre db != nullptr && query != nullptr
 * @post Returns valid Table* with query results or nullptr on failure
 */
Table* query_execute(Database* db, const Query* query);

/**
 * @brief Apply WHERE conditions to a table
 * @param table Input table
 * @param conditions Vector of conditions
 * @return Pointer to filtered table, or nullptr on failure
 */
Table* query_apply_where(Table* table, const std::vector<QueryCondition>& conditions);

/**
 * @brief Apply SELECT projection to a table
 * @param table Input table
 * @param column_names Vector of column names to select
 * @return Pointer to projected table, or nullptr on failure
 */
Table* query_apply_select(Table* table, const std::vector<std::string>& column_names);

/**
 * @brief Apply ORDER BY to a table
 * @param table Input table
 * @param column_names Vector of column names for ordering
 * @param ascending True for ASC, false for DESC
 * @return Pointer to sorted table, or nullptr on failure
 */
Table* query_apply_order_by(Table* table, const std::vector<std::string>& column_names, bool ascending);

/**
 * @brief Apply LIMIT and OFFSET to a table
 * @param table Input table
 * @param limit Limit value (-1 for no limit)
 * @param offset Offset value
 * @return Pointer to limited table, or nullptr on failure
 */
Table* query_apply_limit(Table* table, int limit, int offset);

#endif // QUERY_ENGINE_HPP

