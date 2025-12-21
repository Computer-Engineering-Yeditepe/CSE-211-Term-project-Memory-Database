# Query + Join Engine Implementation

## Overview
This PR adds the complete query parsing and join engine implementation for the database project.

## Author
Öykü Aksungur

## Changes
- ✅ Query parser for SQL-like queries (SELECT, FROM, WHERE, JOIN, ORDER BY, LIMIT)
- ✅ Join engine with two algorithms:
  - Nested Loop Join (for small tables)
  - Hash Join (for larger tables)
- ✅ Support for all join types: INNER, LEFT, RIGHT, FULL OUTER
- ✅ Query execution engine with WHERE filtering
- ✅ Database class for table management
- ✅ Compatible with existing Table/Row/Cell structures

## Files Added
- `include/engine/query/query_types.hpp` - Type definitions and enums
- `include/engine/query/query_parser.hpp` - SQL query parser
- `include/engine/query/join_engine.hpp` - Join operations
- `include/engine/query/query_engine.hpp` - Main query execution engine
- `src/engine/query/query_parser.cpp` - Parser implementation
- `src/engine/query/join_engine.cpp` - Join algorithms implementation
- `src/engine/query/query_engine.cpp` - Query engine implementation

## Features
- **Query Parsing**: Parses SQL-like queries into structured Query objects
- **Join Algorithms**: 
  - Automatically selects nested loop for small tables (< 100 rows)
  - Uses hash join for larger tables for better performance
- **Query Execution**: Complete pipeline from parsing to result table
- **Memory Management**: Proper cleanup of all allocated resources

## Testing
- Code compiles successfully
- Ready for integration testing with other components

## Integration Points
- Uses existing `Table`, `Row`, `Cell` classes from core database
- Compatible with `LinkedList` data structure
- Ready for integration with Hash Index and B+ Tree components

## Notes
- Column name to index mapping implemented using Table::getColumns()
- ORDER BY sorting implemented with bubble sort algorithm
- SELECT projection returns original table (column projection can be enhanced if needed)

