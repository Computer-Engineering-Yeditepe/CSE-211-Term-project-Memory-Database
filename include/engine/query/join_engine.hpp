/**
 * @file join_engine.hpp
 * @author Öykü Aksungur
 * @brief Join operations engine
 * @date 2025
 */

#ifndef JOIN_ENGINE_HPP
#define JOIN_ENGINE_HPP

#include "../core/Table.hpp"
#include "query_types.hpp"
#include <string>

/**
 * @brief Perform nested loop join between two tables
 * @param left_table Left table for join
 * @param right_table Right table for join
 * @param left_column_index Column index in left table
 * @param right_column_index Column index in right table
 * @param join_type Type of join to perform
 * @return Pointer to result table, or nullptr on failure
 * 
 * @pre left_table != nullptr && right_table != nullptr
 * @pre left_column_index >= 0
 * @pre right_column_index >= 0
 * @post Returns valid Table* with joined results or nullptr on failure
 */
Table* join_nested_loop(Table* left_table, Table* right_table,
                        int left_column_index, int right_column_index,
                        JoinType join_type);

/**
 * @brief Perform hash join between two tables
 * @param left_table Left table for join
 * @param right_table Right table for join
 * @param left_column_index Column index in left table
 * @param right_column_index Column index in right table
 * @param join_type Type of join to perform
 * @return Pointer to result table, or nullptr on failure
 * 
 * @pre left_table != nullptr && right_table != nullptr
 * @pre left_column_index >= 0
 * @pre right_column_index >= 0
 * @post Returns valid Table* with joined results or nullptr on failure
 */
Table* join_hash(Table* left_table, Table* right_table,
                 int left_column_index, int right_column_index,
                 JoinType join_type);

/**
 * @brief Perform join based on join condition
 * @param left_table Left table for join
 * @param right_table Right table for join
 * @param condition Join condition
 * @return Pointer to result table, or nullptr on failure
 */
Table* join_execute(Table* left_table, Table* right_table,
                    const JoinCondition& condition);

#endif // JOIN_ENGINE_HPP

