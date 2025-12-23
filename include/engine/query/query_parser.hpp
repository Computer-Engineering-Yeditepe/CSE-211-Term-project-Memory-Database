#ifndef QUERY_PARSER_HPP
#define QUERY_PARSER_HPP

#include "query_types.hpp"
#include <string>

Query* query_parse(const std::string& query_string);
void query_destroy(Query* query);
void query_print(const Query* query);

#endif

