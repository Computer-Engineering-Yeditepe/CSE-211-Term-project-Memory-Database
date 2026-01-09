#ifndef JOIN_ENGINE_HPP
#define JOIN_ENGINE_HPP

#include "core/Table.hpp"
#include "query_types.hpp"
#include <string>

Table* join_nested_loop(Table* left_table, Table* right_table,
                        int left_column_index, int right_column_index,
                        JoinType join_type);

Table* join_hash(Table* left_table, Table* right_table,
                 int left_column_index, int right_column_index,
                 JoinType join_type);

Table* join_execute(Table* left_table, Table* right_table,
                    const JoinCondition& condition);

#endif
