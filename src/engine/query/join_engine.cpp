#include "../../include/engine/query/join_engine.hpp"
#include "../../include/core/Table.hpp"
#include "../../include/core/Row.hpp"
#include "../../include/core/Cell.hpp"
#include <map>
#include <string>

struct HashEntry {
    Row* row;
    HashEntry* next;
    
    HashEntry(Row* r) : row(r), next(nullptr) {}
};

static size_t hash_string(const std::string& str, size_t bucket_count) {
    size_t hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % bucket_count;
}

Table* join_nested_loop(Table* left_table, Table* right_table,
                        int left_column_index, int right_column_index,
                        JoinType join_type) {
    if (!left_table || !right_table) {
        return nullptr;
    }
    
    Table* result = new Table("join_result");
    
    for (auto left_it = left_table->getRows().begin(); left_it != left_table->getRows().end(); ++left_it) {
        Row* left_row = *left_it;
        Cell* left_cell = left_row->getCell(left_column_index);
        if (!left_cell) continue;
        
        std::string left_key;
        if (left_cell->getType() == CellType::INT) {
            left_key = std::to_string(left_cell->getInt());
        } else if (left_cell->getType() == CellType::STRING) {
            left_key = left_cell->getString();
        } else {
            continue;
        }
        
        bool matched = false;
        
        for (auto right_it = right_table->getRows().begin(); right_it != right_table->getRows().end(); ++right_it) {
            Row* right_row = *right_it;
            Cell* right_cell = right_row->getCell(right_column_index);
            if (!right_cell) continue;
            
            std::string right_key;
            if (right_cell->getType() == CellType::INT) {
                right_key = std::to_string(right_cell->getInt());
            } else if (right_cell->getType() == CellType::STRING) {
                right_key = right_cell->getString();
            } else {
                continue;
            }
            
            if (left_key == right_key) {
                Row* joined_row = new Row(left_row->getId());
                
                for (auto cell_it = left_row->getCells().begin(); cell_it != left_row->getCells().end(); ++cell_it) {
                    Cell* cell = *cell_it;
                    if (cell->getType() == CellType::INT) {
                        joined_row->addCell(cell->getInt());
                    } else if (cell->getType() == CellType::DOUBLE) {
                        joined_row->addCell(cell->getDouble());
                    } else if (cell->getType() == CellType::STRING) {
                        joined_row->addCell(cell->getString());
                    }
                }
                
                for (auto cell_it = right_row->getCells().begin(); cell_it != right_row->getCells().end(); ++cell_it) {
                    Cell* cell = *cell_it;
                    if (cell->getType() == CellType::INT) {
                        joined_row->addCell(cell->getInt());
                    } else if (cell->getType() == CellType::DOUBLE) {
                        joined_row->addCell(cell->getDouble());
                    } else if (cell->getType() == CellType::STRING) {
                        joined_row->addCell(cell->getString());
                    }
                }
                
                result->insert(joined_row);
                matched = true;
            }
        }
        
        if (!matched && (join_type == JoinType::LEFT || join_type == JoinType::FULL)) {
            Row* left_only_row = new Row(left_row->getId());
            for (auto cell_it = left_row->getCells().begin(); cell_it != left_row->getCells().end(); ++cell_it) {
                Cell* cell = *cell_it;
                if (cell->getType() == CellType::INT) {
                    left_only_row->addCell(cell->getInt());
                } else if (cell->getType() == CellType::DOUBLE) {
                    left_only_row->addCell(cell->getDouble());
                } else if (cell->getType() == CellType::STRING) {
                    left_only_row->addCell(cell->getString());
                }
            }
            for (size_t i = 0; i < right_table->getRowCount(); i++) {
                left_only_row->addCell(std::string("NULL"));
            }
            result->insert(left_only_row);
        }
    }
    
    return result;
}

Table* join_hash(Table* left_table, Table* right_table,
                 int left_column_index, int right_column_index,
                 JoinType join_type) {
    if (!left_table || !right_table) {
        return nullptr;
    }
    
    const size_t bucket_count = 1009;
    HashEntry** hash_table = new HashEntry*[bucket_count]();
    
    for (auto it = left_table->getRows().begin(); it != left_table->getRows().end(); ++it) {
        Row* row = *it;
        Cell* cell = row->getCell(left_column_index);
        if (!cell) continue;
        
        std::string key;
        if (cell->getType() == CellType::INT) {
            key = std::to_string(cell->getInt());
        } else if (cell->getType() == CellType::STRING) {
            key = cell->getString();
        } else {
            continue;
        }
        
        size_t idx = hash_string(key, bucket_count);
        HashEntry* entry = new HashEntry(row);
        entry->next = hash_table[idx];
        hash_table[idx] = entry;
    }
    
    Table* result = new Table("join_result");
    
    for (auto it = right_table->getRows().begin(); it != right_table->getRows().end(); ++it) {
        Row* right_row = *it;
        Cell* right_cell = right_row->getCell(right_column_index);
        if (!right_cell) continue;
        
        std::string key;
        if (right_cell->getType() == CellType::INT) {
            key = std::to_string(right_cell->getInt());
        } else if (right_cell->getType() == CellType::STRING) {
            key = right_cell->getString();
        } else {
            continue;
        }
        
        size_t idx = hash_string(key, bucket_count);
        HashEntry* entry = hash_table[idx];
        
        while (entry) {
            Row* left_row = entry->row;
            Cell* left_cell = left_row->getCell(left_column_index);
            
            if (left_cell) {
                std::string left_key;
                if (left_cell->getType() == CellType::INT) {
                    left_key = std::to_string(left_cell->getInt());
                } else if (left_cell->getType() == CellType::STRING) {
                    left_key = left_cell->getString();
                }
                
                if (left_key == key) {
                    Row* joined_row = new Row(left_row->getId());
                    
                    for (auto cell_it = left_row->getCells().begin(); cell_it != left_row->getCells().end(); ++cell_it) {
                        Cell* cell = *cell_it;
                        if (cell->getType() == CellType::INT) {
                            joined_row->addCell(cell->getInt());
                        } else if (cell->getType() == CellType::DOUBLE) {
                            joined_row->addCell(cell->getDouble());
                        } else if (cell->getType() == CellType::STRING) {
                            joined_row->addCell(cell->getString());
                        }
                    }
                    
                    for (auto cell_it = right_row->getCells().begin(); cell_it != right_row->getCells().end(); ++cell_it) {
                        Cell* cell = *cell_it;
                        if (cell->getType() == CellType::INT) {
                            joined_row->addCell(cell->getInt());
                        } else if (cell->getType() == CellType::DOUBLE) {
                            joined_row->addCell(cell->getDouble());
                        } else if (cell->getType() == CellType::STRING) {
                            joined_row->addCell(cell->getString());
                        }
                    }
                    
                    result->insert(joined_row);
                }
            }
            
            entry = entry->next;
        }
    }
    
    for (size_t i = 0; i < bucket_count; i++) {
        HashEntry* entry = hash_table[i];
        while (entry) {
            HashEntry* next = entry->next;
            delete entry;
            entry = next;
        }
    }
    delete[] hash_table;
    
    return result;
}

Table* join_execute(Table* left_table, Table* right_table,
                    const JoinCondition& condition) {
    if (!left_table || !right_table) {
        return nullptr;
    }
    
    int left_col_idx = -1;
    const LinkedList<std::string>& left_columns = left_table->getColumns();
    int idx = 0;
    for (auto it = left_columns.begin(); it != left_columns.end(); ++it) {
        if (*it == condition.left_column) {
            left_col_idx = idx;
            break;
        }
        idx++;
    }
    
    int right_col_idx = -1;
    const LinkedList<std::string>& right_columns = right_table->getColumns();
    idx = 0;
    for (auto it = right_columns.begin(); it != right_columns.end(); ++it) {
        if (*it == condition.right_column) {
            right_col_idx = idx;
            break;
        }
        idx++;
    }
    
    if (left_col_idx < 0) left_col_idx = 0;
    if (right_col_idx < 0) right_col_idx = 0;
    
    if (left_table->getRowCount() < 100 && right_table->getRowCount() < 100) {
        return join_nested_loop(left_table, right_table, left_col_idx, right_col_idx, condition.join_type);
    } else {
        return join_hash(left_table, right_table, left_col_idx, right_col_idx, condition.join_type);
    }
}

