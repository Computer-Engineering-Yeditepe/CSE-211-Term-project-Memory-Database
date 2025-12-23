#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include "../core/Table.hpp"
#include "query_types.hpp"
#include "../../data_structures/LinkedList.hpp"
#include "../../data_structures/Map.hpp"
#include <string>

class Database {
private:
    Map<std::string, Table*> tables;

public:
    Database();
    ~Database();
    int addTable(Table* table);
    Table* getTable(const std::string& table_name);
    LinkedList<std::string> getTableNames() const;
};

Table* query_execute(Database* db, const Query* query);
Table* query_apply_where(Table* table, const LinkedList<QueryCondition>& conditions);
Table* query_apply_select(Table* table, const LinkedList<std::string>& column_names);
Table* query_apply_order_by(Table* table, const LinkedList<std::string>& column_names, bool ascending);
Table* query_apply_limit(Table* table, int limit, int offset);

#endif

