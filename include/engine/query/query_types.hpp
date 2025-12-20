/**
 * @file query_types.hpp
 * @author Öykü Aksungur
 * @brief Type definitions and enums for query engine
 * @date 2025
 */

#ifndef QUERY_TYPES_HPP
#define QUERY_TYPES_HPP

#include <string>
#include <vector>

/**
 * @enum JoinType
 * @brief Types of join operations supported
 */
enum class JoinType {
    INNER,      ///< Inner join
    LEFT,       ///< Left outer join
    RIGHT,      ///< Right outer join
    FULL        ///< Full outer join
};

/**
 * @enum ComparisonOperator
 * @brief Comparison operators for WHERE clauses
 */
enum class ComparisonOperator {
    EQUAL,          ///< =
    NOT_EQUAL,      ///< !=
    LESS_THAN,      ///< <
    LESS_EQUAL,     ///< <=
    GREATER_THAN,   ///< >
    GREATER_EQUAL,  ///< >=
    LIKE            ///< LIKE (pattern matching)
};

/**
 * @enum LogicalOperator
 * @brief Logical operators for combining conditions
 */
enum class LogicalOperator {
    AND,    ///< AND
    OR,     ///< OR
    NOT     ///< NOT
};

/**
 * @struct QueryCondition
 * @brief Represents a single condition in WHERE clause
 */
struct QueryCondition {
    std::string column_name;      ///< Column name to compare
    ComparisonOperator op;       ///< Comparison operator
    std::string value;            ///< Value to compare against
    LogicalOperator logical_op;   ///< Logical operator (AND/OR) for chaining
};

/**
 * @struct JoinCondition
 * @brief Represents a join condition between two tables
 */
struct JoinCondition {
    std::string left_table;   ///< Left table name
    std::string left_column;  ///< Left column name
    std::string right_table;  ///< Right table name
    std::string right_column; ///< Right column name
    JoinType join_type;       ///< Type of join
};

/**
 * @struct Query
 * @brief Represents a parsed SQL query
 */
struct Query {
    std::vector<std::string> select_columns;  ///< Column names to select
    std::vector<std::string> from_tables;     ///< Table names
    std::vector<QueryCondition> conditions;   ///< WHERE conditions
    std::vector<JoinCondition> joins;          ///< Join conditions
    std::vector<std::string> order_by;        ///< Columns for ORDER BY
    bool order_asc;                            ///< True for ASC, false for DESC
    int limit;                                 ///< LIMIT value (-1 if not specified)
    int offset;                                ///< OFFSET value (0 if not specified)
};

#endif // QUERY_TYPES_HPP

