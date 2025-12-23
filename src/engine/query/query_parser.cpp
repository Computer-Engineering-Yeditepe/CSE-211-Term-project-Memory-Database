#include "../../include/engine/query/query_parser.hpp"
#include "../../include/engine/query/query_types.hpp"
#include "../../include/data_structures/LinkedList.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

static LinkedList<std::string> split(const std::string& str, char delimiter) {
    LinkedList<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

static std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

Query* query_parse(const std::string& query_string) {
    if (query_string.empty()) {
        return nullptr;
    }
    
    Query* query = new Query();
    std::string upper_query = toUpper(query_string);
    
    size_t select_pos = upper_query.find("SELECT");
    size_t from_pos = upper_query.find("FROM");
    size_t where_pos = upper_query.find("WHERE");
    size_t join_pos = upper_query.find("JOIN");
    size_t order_pos = upper_query.find("ORDER BY");
    size_t limit_pos = upper_query.find("LIMIT");
    
    if (select_pos != std::string::npos && from_pos != std::string::npos) {
        std::string select_clause = query_string.substr(select_pos + 6, from_pos - select_pos - 6);
        select_clause = trim(select_clause);
        
        if (select_clause == "*") {
            query->select_columns.clear();
        } else {
            LinkedList<std::string> cols = split(select_clause, ',');
            query->select_columns.clear();
            for (auto it = cols.begin(); it != cols.end(); ++it) {
                query->select_columns.push_back(*it);
            }
        }
    }
    
    if (from_pos != std::string::npos) {
        size_t from_end = where_pos;
        if (from_end == std::string::npos) from_end = join_pos;
        if (from_end == std::string::npos) from_end = order_pos;
        if (from_end == std::string::npos) from_end = limit_pos;
        if (from_end == std::string::npos) from_end = query_string.length();
        
        std::string from_clause = query_string.substr(from_pos + 4, from_end - from_pos - 4);
        from_clause = trim(from_clause);
        LinkedList<std::string> tables = split(from_clause, ',');
        query->from_tables.clear();
        for (auto it = tables.begin(); it != tables.end(); ++it) {
            query->from_tables.push_back(*it);
        }
    }
    
    if (where_pos != std::string::npos) {
        size_t where_end = join_pos;
        if (where_end == std::string::npos) where_end = order_pos;
        if (where_end == std::string::npos) where_end = limit_pos;
        if (where_end == std::string::npos) where_end = query_string.length();
        
        std::string where_clause = query_string.substr(where_pos + 5, where_end - where_pos - 5);
        where_clause = trim(where_clause);
        
        size_t eq_pos = where_clause.find('=');
        if (eq_pos != std::string::npos) {
            QueryCondition condition;
            condition.column_name = trim(where_clause.substr(0, eq_pos));
            condition.value = trim(where_clause.substr(eq_pos + 1));
            condition.op = ComparisonOperator::EQUAL;
            condition.logical_op = LogicalOperator::AND;
            query->conditions.push_back(condition);
        }
    }
    
    query->order_asc = true;
    query->limit = -1;
    query->offset = 0;
    
    return query;
}

void query_destroy(Query* query) {
    if (query) {
        delete query;
    }
}

void query_print(const Query* query) {
    if (!query) return;
    
    std::cout << "Query:\n";
    std::cout << "  SELECT: ";
    if (query->select_columns.empty()) {
        std::cout << "*\n";
    } else {
        for (auto it = query->select_columns.begin(); it != query->select_columns.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";
    }
    
    std::cout << "  FROM: ";
    for (auto it = query->from_tables.begin(); it != query->from_tables.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    if (!query->conditions.empty()) {
        std::cout << "  WHERE: ";
        for (auto it = query->conditions.begin(); it != query->conditions.end(); ++it) {
            const QueryCondition& cond = *it;
            std::cout << cond.column_name << " = " << cond.value << " ";
        }
        std::cout << "\n";
    }
}

