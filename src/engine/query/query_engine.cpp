#include "../../include/engine/query/query_engine.hpp"
#include "../../include/engine/query/join_engine.hpp"
#include "../../include/core/Table.hpp"
#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp"
#include <algorithm>

Database::Database() {}

Database::~Database() {
}

int Database::addTable(Table* table) {
    if (!table) return -1;
    tables.insert(table->getName(), table);
    return 0;
}

Table* Database::getTable(const std::string& table_name) {
    Table** found = tables.find(table_name);
    if (found != nullptr) {
        return *found;
    }
    return nullptr;
}

LinkedList<std::string> Database::getTableNames() const {
    LinkedList<std::string> names;
    for (auto it = tables.begin(); it != tables.end(); ++it) {
        names.push_back((*it).key);
    }
    return names;
}

static int get_column_index_by_name(Table* table, const std::string& column_name) {
    if (!table) return -1;
    
    const LinkedList<std::string>& columns = table->getColumns();
    int index = 0;
    for (auto it = columns.begin(); it != columns.end(); ++it) {
        if (*it == column_name) {
            return index;
        }
        index++;
    }
    return -1;
}

static bool evaluate_condition(Row* row, const QueryCondition& condition, Table* table) {
    if (!row || !table) return false;
    
    int col_idx = get_column_index_by_name(table, condition.column_name);
    if (col_idx < 0) return false;
    
    Cell* cell = row->getCell(col_idx);
    if (!cell) return false;
    
    std::string cell_value;
    if (cell->getType() == CellType::INT) {
        cell_value = std::to_string(cell->getInt());
    } else if (cell->getType() == CellType::STRING) {
        cell_value = cell->getString();
    } else {
        return false;
    }
    
    switch (condition.op) {
        case ComparisonOperator::EQUAL:
            return cell_value == condition.value;
        case ComparisonOperator::NOT_EQUAL:
            return cell_value != condition.value;
        case ComparisonOperator::LESS_THAN:
            return std::stoi(cell_value) < std::stoi(condition.value);
        case ComparisonOperator::LESS_EQUAL:
            return std::stoi(cell_value) <= std::stoi(condition.value);
        case ComparisonOperator::GREATER_THAN:
            return std::stoi(cell_value) > std::stoi(condition.value);
        case ComparisonOperator::GREATER_EQUAL:
            return std::stoi(cell_value) >= std::stoi(condition.value);
        case ComparisonOperator::LIKE:
            return cell_value.find(condition.value) != std::string::npos;
        default:
            return false;
    }
}

Table* query_apply_where(Table* table, const LinkedList<QueryCondition>& conditions) {
    if (!table || conditions.empty()) {
        return table;
    }
    
    LinkedList<std::string> empty_cols;
    LinkedList<std::string> empty_types;
    Table* result = new Table(table->getName() + "_filtered", empty_cols, empty_types);
    
    for (auto it = table->getRows().begin(); it != table->getRows().end(); ++it) {
        Row* row = *it;
        bool matches = true;
        
        for (auto cond_it = conditions.begin(); cond_it != conditions.end(); ++cond_it) {
            const QueryCondition& condition = *cond_it;
            bool condition_result = evaluate_condition(row, condition, table);
            
            if (condition.logical_op == LogicalOperator::AND) {
                matches = matches && condition_result;
            } else if (condition.logical_op == LogicalOperator::OR) {
                matches = matches || condition_result;
            }
        }
        
        if (matches) {
            Row* new_row = new Row(row->getId());
            for (auto cell_it = row->getCells().begin(); cell_it != row->getCells().end(); ++cell_it) {
                Cell* cell = *cell_it;
                if (cell->getType() == CellType::INT) {
                    new_row->addCell(cell->getInt());
                } else if (cell->getType() == CellType::DOUBLE) {
                    new_row->addCell(cell->getDouble());
                } else if (cell->getType() == CellType::STRING) {
                    new_row->addCell(cell->getString());
                }
            }
            result->insertRow(new_row);
        }
    }
    
    return result;
}

Table* query_apply_select(Table* table, const LinkedList<std::string>& column_names) {
    if (!table || column_names.empty()) {
        return table;
    }
    
    LinkedList<int> col_indices;
    const LinkedList<std::string>& all_columns = table->getColumns();
    
    for (auto it = column_names.begin(); it != column_names.end(); ++it) {
        const std::string& col_name = *it;
        int idx = get_column_index_by_name(table, col_name);
        if (idx >= 0) {
            col_indices.push_back(idx);
        }
    }
    
    if (col_indices.empty()) {
        return table;
    }
    
    return table;
}

static bool compare_rows(Row* row1, Row* row2, int col_idx, bool ascending) {
    Cell* cell1 = row1->getCell(col_idx);
    Cell* cell2 = row2->getCell(col_idx);
    
    if (!cell1 || !cell2) return false;
    
    bool result = false;
    if (cell1->getType() == CellType::INT && cell2->getType() == CellType::INT) {
        result = cell1->getInt() < cell2->getInt();
    } else if (cell1->getType() == CellType::DOUBLE && cell2->getType() == CellType::DOUBLE) {
        result = cell1->getDouble() < cell2->getDouble();
    } else if (cell1->getType() == CellType::STRING && cell2->getType() == CellType::STRING) {
        result = cell1->getString() < cell2->getString();
    }
    
    return ascending ? result : !result;
}

Table* query_apply_order_by(Table* table, const LinkedList<std::string>& column_names, bool ascending) {
    if (!table || column_names.empty()) {
        return table;
    }
    
    auto first_col_it = column_names.begin();
    if (first_col_it == column_names.end()) {
        return table;
    }
    
    int col_idx = get_column_index_by_name(table, *first_col_it);
    if (col_idx < 0) {
        return table;
    }
    
    LinkedList<Row*> rows_list;
    for (auto it = table->getRows().begin(); it != table->getRows().end(); ++it) {
        rows_list.push_back(*it);
    }
    
    size_t list_size = rows_list.size();
    for (size_t i = 0; i < list_size; i++) {
        auto it1 = rows_list.begin();
        auto it2 = rows_list.begin();
        ++it2;
        
        for (size_t j = 0; j < list_size - i - 1 && it2 != rows_list.end(); j++) {
            Row* row1 = *it1;
            Row* row2 = *it2;
            if (compare_rows(row1, row2, col_idx, ascending)) {
                Row* temp = *it1;
                *it1 = *it2;
                *it2 = temp;
            }
            ++it1;
            ++it2;
        }
    }
    
    LinkedList<std::string> empty_cols;
    LinkedList<std::string> empty_types;
    Table* sorted_table = new Table(table->getName() + "_sorted", empty_cols, empty_types);
    
    for (auto it = rows_list.begin(); it != rows_list.end(); ++it) {
        Row* row = *it;
        Row* new_row = new Row(row->getId());
        for (auto cell_it = row->getCells().begin(); cell_it != row->getCells().end(); ++cell_it) {
            Cell* cell = *cell_it;
            if (cell->getType() == CellType::INT) {
                new_row->addCell(cell->getInt());
            } else if (cell->getType() == CellType::DOUBLE) {
                new_row->addCell(cell->getDouble());
            } else if (cell->getType() == CellType::STRING) {
                new_row->addCell(cell->getString());
            }
        }
        sorted_table->insertRow(new_row);
    }
    
    return sorted_table;
}

Table* query_apply_limit(Table* table, int limit, int offset) {
    if (!table) return nullptr;
    
    if (limit < 0 && offset == 0) {
        return table;
    }
    
    LinkedList<std::string> empty_cols;
    LinkedList<std::string> empty_types;
    Table* result = new Table(table->getName() + "_limited", empty_cols, empty_types);
    int skipped = 0;
    int taken = 0;
    
    for (auto it = table->getRows().begin(); it != table->getRows().end(); ++it) {
        if (skipped < offset) {
            skipped++;
            continue;
        }
        
        if (limit < 0 || taken < limit) {
            Row* row = *it;
            Row* new_row = new Row(row->getId());
            for (auto cell_it = row->getCells().begin(); cell_it != row->getCells().end(); ++cell_it) {
                Cell* cell = *cell_it;
                if (cell->getType() == CellType::INT) {
                    new_row->addCell(cell->getInt());
                } else if (cell->getType() == CellType::DOUBLE) {
                    new_row->addCell(cell->getDouble());
                } else if (cell->getType() == CellType::STRING) {
                    new_row->addCell(cell->getString());
                }
            }
            result->insertRow(new_row);
            taken++;
        } else {
            break;
        }
    }
    
    return result;
}

Table* query_execute(Database* db, const Query* query) {
    if (!db || !query || query->from_tables.empty()) {
        return nullptr;
    }
    
    auto first_table_it = query->from_tables.begin();
    if (first_table_it == query->from_tables.end()) {
        return nullptr;
    }
    
    Table* result = db->getTable(*first_table_it);
    if (!result) {
        return nullptr;
    }
    
    if (!query->joins.empty()) {
        for (auto it = query->joins.begin(); it != query->joins.end(); ++it) {
            const JoinCondition& join = *it;
            Table* right_table = db->getTable(join.right_table);
            if (right_table) {
                Table* joined = join_execute(result, right_table, join);
                if (joined && joined != result) {
                    result = joined;
                }
            }
        }
    }
    
    if (!query->conditions.empty()) {
        Table* filtered = query_apply_where(result, query->conditions);
        if (filtered && filtered != result) {
            result = filtered;
        }
    }
    
    if (!query->select_columns.empty()) {
        Table* projected = query_apply_select(result, query->select_columns);
        if (projected && projected != result) {
            result = projected;
        }
    }
    
    if (!query->order_by.empty()) {
        Table* sorted = query_apply_order_by(result, query->order_by, query->order_asc);
        if (sorted && sorted != result) {
            result = sorted;
        }
    }
    
    if (query->limit >= 0 || query->offset > 0) {
        Table* limited = query_apply_limit(result, query->limit, query->offset);
        if (limited && limited != result) {
            result = limited;
        }
    }
    
    return result;
}

