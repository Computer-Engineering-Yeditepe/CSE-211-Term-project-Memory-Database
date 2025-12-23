#ifndef QUERY_TYPES_HPP
#define QUERY_TYPES_HPP

#include <string>
#include "../../data_structures/LinkedList.hpp"

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
    LinkedList<std::string> select_columns;
    LinkedList<std::string> from_tables;
    LinkedList<QueryCondition> conditions;
    LinkedList<JoinCondition> joins;
    LinkedList<std::string> order_by;
    bool order_asc;
    int limit;
    int offset;
};

#endif

