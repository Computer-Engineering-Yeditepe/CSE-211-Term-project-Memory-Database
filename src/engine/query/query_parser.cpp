/**
 * src/engine/query/query_parser.cpp
 * JOIN PARSING EKLENMİŞ SÜRÜM
 */
#include "../../../include/engine/query/query_parser.hpp"
#include "../../../include/engine/query/query_types.hpp"
#include "../../../include/data_structures/LinkedList.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
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
    if (query_string.empty()) return nullptr;
    
    Query* query = new Query();
    std::string upper_query = toUpper(query_string);
    
    size_t select_pos = upper_query.find("SELECT");
    size_t from_pos = upper_query.find("FROM");
    size_t where_pos = upper_query.find("WHERE");
    size_t join_pos = upper_query.find("JOIN");
    size_t order_pos = upper_query.find("ORDER BY");
    size_t limit_pos = upper_query.find("LIMIT");
    
    // 1. SELECT
    if (select_pos != std::string::npos && from_pos != std::string::npos) {
        std::string select_clause = query_string.substr(select_pos + 6, from_pos - select_pos - 6);
        LinkedList<std::string> cols = split(select_clause, ',');
        for (auto it = cols.begin(); it != cols.end(); ++it) {
            std::string c = *it;
            if (c != "*") query->select_columns.push_back(c);
        }
    }
    
    // 2. FROM
    if (from_pos != std::string::npos) {
        size_t from_end = where_pos;
        // Eğer JOIN varsa FROM orada biter
        if (join_pos != std::string::npos && (from_end == std::string::npos || join_pos < from_end)) from_end = join_pos;
        if (from_end == std::string::npos) from_end = order_pos;
        if (from_end == std::string::npos) from_end = limit_pos;
        if (from_end == std::string::npos) from_end = query_string.length();
        
        std::string from_clause = query_string.substr(from_pos + 4, from_end - from_pos - 4);
        query->from_tables.push_back(trim(from_clause));
    }

    // 3. JOIN (YENİ EKLENEN KISIM)
    if (join_pos != std::string::npos) {
        size_t on_pos = upper_query.find("ON", join_pos);
        if (on_pos != std::string::npos) {
            // Tablo Adı: JOIN ile ON arası
            std::string table_part = query_string.substr(join_pos + 4, on_pos - (join_pos + 4));
            
            // Koşul: ON ile sonraki keyword arası
            size_t cond_start = on_pos + 2;
            size_t cond_end = where_pos;
            if (cond_end == std::string::npos) cond_end = order_pos;
            if (cond_end == std::string::npos) cond_end = limit_pos;
            if (cond_end == std::string::npos) cond_end = query_string.length();
            
            std::string cond_str = query_string.substr(cond_start, cond_end - cond_start);
            cond_str = trim(cond_str);
            
            // "ID = UserID" gibi bir ifadeyi parçala
            size_t eq_pos = cond_str.find('=');
            if (eq_pos != std::string::npos) {
                JoinCondition join;
                join.right_table = trim(table_part);
                join.left_column = trim(cond_str.substr(0, eq_pos));  // Örn: ID
                join.right_column = trim(cond_str.substr(eq_pos + 1)); // Örn: UserID
                join.join_type = JoinType::INNER;
                query->joins.push_back(join);
            }
        }
    }
    
    // 4. WHERE
    if (where_pos != std::string::npos) {
        size_t where_end = order_pos; 
        if (where_end == std::string::npos) where_end = limit_pos;
        if (where_end == std::string::npos) where_end = query_string.length();
        
        std::string where_clause = query_string.substr(where_pos + 5, where_end - where_pos - 5);
        where_clause = trim(where_clause);
        
        // Basit Operatör kontrolü
        std::string opStr = "=";
        ComparisonOperator opEnum = ComparisonOperator::EQUAL;
        size_t opPos = std::string::npos;
        
        if ((opPos = where_clause.find(">=")) != std::string::npos) { opStr = ">="; opEnum = ComparisonOperator::GREATER_EQUAL; }
        else if ((opPos = where_clause.find("<=")) != std::string::npos) { opStr = "<="; opEnum = ComparisonOperator::LESS_EQUAL; }
        else if ((opPos = where_clause.find("!=")) != std::string::npos) { opStr = "!="; opEnum = ComparisonOperator::NOT_EQUAL; }
        else if ((opPos = where_clause.find("=")) != std::string::npos) { opStr = "="; opEnum = ComparisonOperator::EQUAL; }
        else if ((opPos = where_clause.find(">")) != std::string::npos) { opStr = ">"; opEnum = ComparisonOperator::GREATER_THAN; }
        else if ((opPos = where_clause.find("<")) != std::string::npos) { opStr = "<"; opEnum = ComparisonOperator::LESS_THAN; }

        if (opPos != std::string::npos) {
            QueryCondition condition;
            condition.column_name = trim(where_clause.substr(0, opPos));
            condition.value = trim(where_clause.substr(opPos + opStr.length()));
            condition.op = opEnum;
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
    if (query) delete query;
}