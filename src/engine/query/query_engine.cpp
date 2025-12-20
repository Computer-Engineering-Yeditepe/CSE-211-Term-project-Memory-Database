/**
 * @file query_engine.cpp
 * @author Öykü Aksungur
 * @brief Implementation of main query execution engine
 * @date 2025
 */

#include "../../include/engine/query/query_engine.hpp"
#include "../../include/engine/query/join_engine.hpp"
#include "../../include/core/Table.hpp"
#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp"
#include <algorithm>

Database::Database() {}

Database::~Database() {
    // Tables are managed externally, don't delete here
}

int Database::addTable(Table* table) {
    if (!table) return -1;
    tables[table->getName()] = table;
    return 0;
}

Table* Database::getTable(const std::string& table_name) {
    auto it = tables.find(table_name);
    if (it != tables.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> Database::getTableNames() const {
    std::vector<std::string> names;
    for (const auto& pair : tables) {
        names.push_back(pair.first);
    }
    return names;
}

static bool evaluate_condition(Row* row, const QueryCondition& condition, Table* table) {
    if (!row || !table) return false;
    
    // Get cell by index (simplified - assumes column index matches cell index)
    // In a real implementation, you'd need a column name to index mapping
    Cell* cell = row->getCell(0); // Simplified - use first cell
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

Table* query_apply_where(Table* table, const std::vector<QueryCondition>& conditions) {
    if (!table || conditions.empty()) {
        return table;
    }
    
    Table* result = new Table(table->getName() + "_filtered");
    
    for (auto it = table->getRows().begin(); it != table->getRows().end(); ++it) {
        Row* row = *it;
        bool matches = true;
        
        for (const auto& condition : conditions) {
            bool condition_result = evaluate_condition(row, condition, table);
            
            if (condition.logical_op == LogicalOperator::AND) {
                matches = matches && condition_result;
            } else if (condition.logical_op == LogicalOperator::OR) {
                matches = matches || condition_result;
            }
        }
        
        if (matches) {
            // Create a copy of the row
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
            result->insert(new_row);
        }
    }
    
    return result;
}

Table* query_apply_select(Table* table, const std::vector<std::string>& column_names) {
    if (!table || column_names.empty()) {
        return table; // Select all
    }
    
    // Simplified - in real implementation, you'd need column mapping
    return table;
}

Table* query_apply_order_by(Table* table, const std::vector<std::string>& column_names, bool ascending) {
    // Placeholder - full sorting implementation needed
    return table;
}

Table* query_apply_limit(Table* table, int limit, int offset) {
    if (!table) return nullptr;
    
    if (limit < 0 && offset == 0) {
        return table; // No limit
    }
    
    Table* result = new Table(table->getName() + "_limited");
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
            result->insert(new_row);
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
    
    // Get first table
    Table* result = db->getTable(query->from_tables[0]);
    if (!result) {
        return nullptr;
    }
    
    // Apply joins
    if (!query->joins.empty()) {
        for (const auto& join : query->joins) {
            Table* right_table = db->getTable(join.right_table);
            if (right_table) {
                Table* joined = join_execute(result, right_table, join);
                if (joined && joined != result) {
                    result = joined;
                }
            }
        }
    }
    
    // Apply WHERE
    if (!query->conditions.empty()) {
        Table* filtered = query_apply_where(result, query->conditions);
        if (filtered && filtered != result) {
            result = filtered;
        }
    }
    
    // Apply SELECT
    if (!query->select_columns.empty()) {
        Table* projected = query_apply_select(result, query->select_columns);
        if (projected && projected != result) {
            result = projected;
        }
    }
    
    // Apply ORDER BY
    if (!query->order_by.empty()) {
        Table* sorted = query_apply_order_by(result, query->order_by, query->order_asc);
        if (sorted && sorted != result) {
            result = sorted;
        }
    }
    
    // Apply LIMIT
    if (query->limit >= 0 || query->offset > 0) {
        Table* limited = query_apply_limit(result, query->limit, query->offset);
        if (limited && limited != result) {
            result = limited;
        }
    }
    
    return result;
}

