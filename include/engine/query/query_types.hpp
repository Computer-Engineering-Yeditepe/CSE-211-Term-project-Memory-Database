#ifndef QUERY_TYPES_HPP
#define QUERY_TYPES_HPP

#include <string>
#include <vector>

enum class JoinType {
    INNER,
    LEFT,
    RIGHT,
    FULL
};

enum class ComparisonOperator {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    LIKE
};

enum class LogicalOperator {
    AND,
    OR,
    NOT
};

struct QueryCondition {
    std::string column_name;
    ComparisonOperator op;
    std::string value;
    LogicalOperator logical_op;
};

struct JoinCondition {
    std::string left_table;
    std::string left_column;
    std::string right_table;
    std::string right_column;
    JoinType join_type;
};

struct Query {
    std::vector<std::string> select_columns;
    std::vector<std::string> from_tables;
    std::vector<QueryCondition> conditions;
    std::vector<JoinCondition> joins;
    std::vector<std::string> order_by;
    bool order_asc;
    int limit;
    int offset;
};

#endif // QUERY_TYPES_HPP

