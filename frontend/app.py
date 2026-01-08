"""
In-Memory Database Engine - Flask API
CSE 211 - Data Structures Project

Bu API, C++ backend'inize bağlanmak için bir köprü görevi görür.
Şimdilik Python'da simüle ediyoruz, sonra C++ fonksiyonlarınızı çağırabilirsiniz.

Çalıştırmak için:
    pip install flask flask-cors
    python app.py

Tarayıcıda: http://localhost:5000
"""

from flask import Flask, jsonify, request, render_template_string
from flask_cors import CORS
import time
import random

app = Flask(__name__)
CORS(app)

# ============================================================================
# DATABASE STATE (Simulated - Replace with C++ calls later)
# ============================================================================

database = {
    "users": {
        "name": "users",
        "columns": ["id", "name", "age"],
        "types": ["INT", "STRING", "INT"],
        "primary_key": "id",
        "rows": [
            {"id": 1, "name": "Alice", "age": 25},
            {"id": 2, "name": "Bob", "age": 30},
            {"id": 3, "name": "Carol", "age": 28},
        ],
        "indexes": {"btree": ["id"], "hash": ["name"]}
    },
    "orders": {
        "name": "orders",
        "columns": ["order_id", "user_id", "amount"],
        "types": ["INT", "INT", "DOUBLE"],
        "primary_key": "order_id",
        "rows": [
            {"order_id": 101, "user_id": 1, "amount": 99.99},
            {"order_id": 102, "user_id": 2, "amount": 149.50},
            {"order_id": 103, "user_id": 1, "amount": 29.99},
        ],
        "indexes": {"btree": ["order_id"], "hash": []}
    }
}

# ============================================================================
# API ENDPOINTS
# ============================================================================

@app.route('/')
def index():
    """Ana sayfa - HTML GUI"""
    return render_template_string(HTML_TEMPLATE)


@app.route('/api/tables', methods=['GET'])
def get_tables():
    """Tüm tabloları listele"""
    tables_info = {}
    for name, table in database.items():
        tables_info[name] = {
            "name": table["name"],
            "columns": table["columns"],
            "types": table["types"],
            "primary_key": table["primary_key"],
            "row_count": len(table["rows"]),
            "indexes": table["indexes"]
        }
    return jsonify(tables_info)


@app.route('/api/tables/<table_name>', methods=['GET'])
def get_table(table_name):
    """Belirli bir tabloyu getir"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    return jsonify(database[table_name])


@app.route('/api/tables', methods=['POST'])
def create_table():
    """Yeni tablo oluştur"""
    data = request.json
    name = data.get('name')
    columns = data.get('columns', [])
    types = data.get('types', [])
    
    if not name or not columns:
        return jsonify({"error": "Name and columns required"}), 400
    
    if name in database:
        return jsonify({"error": "Table already exists"}), 400
    
    database[name] = {
        "name": name,
        "columns": columns,
        "types": types,
        "primary_key": columns[0] if columns else None,
        "rows": [],
        "indexes": {"btree": [], "hash": []}
    }
    
    return jsonify({"message": f"Table '{name}' created", "table": database[name]})


@app.route('/api/tables/<table_name>', methods=['DELETE'])
def delete_table(table_name):
    """Tabloyu sil"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    
    del database[table_name]
    return jsonify({"message": f"Table '{table_name}' deleted"})


@app.route('/api/tables/<table_name>/rows', methods=['GET'])
def get_rows(table_name):
    """Tablodaki tüm satırları getir"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    return jsonify(database[table_name]["rows"])


@app.route('/api/tables/<table_name>/rows', methods=['POST'])
def insert_row(table_name):
    """Yeni satır ekle"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    
    row_data = request.json
    table = database[table_name]
    
    # Type conversion
    for i, col in enumerate(table["columns"]):
        if col in row_data:
            if table["types"][i] == "INT":
                row_data[col] = int(row_data[col])
            elif table["types"][i] == "DOUBLE":
                row_data[col] = float(row_data[col])
    
    table["rows"].append(row_data)
    
    # TODO: Burada C++ backend'e bağlanabilirsiniz
    # Örnek: cpp_table.insertRow(row_data)
    
    return jsonify({"message": "Row inserted", "row": row_data})


@app.route('/api/tables/<table_name>/rows/<int:row_index>', methods=['DELETE'])
def delete_row(table_name, row_index):
    """Satır sil"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    
    table = database[table_name]
    if row_index < 0 or row_index >= len(table["rows"]):
        return jsonify({"error": "Row not found"}), 404
    
    deleted_row = table["rows"].pop(row_index)
    
    # TODO: C++ backend bağlantısı
    # Örnek: cpp_table.removeRow(row_index)
    
    return jsonify({"message": "Row deleted", "row": deleted_row})


@app.route('/api/tables/<table_name>/indexes', methods=['POST'])
def create_index(table_name):
    """Index oluştur (B+ Tree veya Hash)"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    
    data = request.json
    column = data.get('column')
    index_type = data.get('type', 'btree')  # 'btree' or 'hash'
    
    if not column:
        return jsonify({"error": "Column required"}), 400
    
    table = database[table_name]
    if column not in table["columns"]:
        return jsonify({"error": "Column not found"}), 400
    
    if index_type not in ['btree', 'hash']:
        return jsonify({"error": "Invalid index type"}), 400
    
    if column not in table["indexes"][index_type]:
        table["indexes"][index_type].append(column)
    
    # TODO: C++ backend bağlantısı
    # if index_type == 'btree':
    #     cpp_table.createBPlusTreeIndex(column)
    # else:
    #     cpp_table.createHashIndex(column)
    
    return jsonify({
        "message": f"{index_type.upper()} index created on '{column}'",
        "indexes": table["indexes"]
    })


@app.route('/api/query', methods=['POST'])
def execute_query():
    """SQL sorgusu çalıştır"""
    data = request.json
    query_string = data.get('query', '')
    
    start_time = time.time()
    
    # Basit sorgu ayrıştırma (gerçek projede query_parser.cpp kullanılacak)
    query_upper = query_string.upper()
    result = {"columns": [], "rows": [], "query_type": ""}
    
    try:
        if "JOIN" in query_upper:
            # JOIN simülasyonu
            result = {
                "columns": ["users.name", "orders.amount"],
                "rows": [
                    {"users.name": "Alice", "orders.amount": 99.99},
                    {"users.name": "Alice", "orders.amount": 29.99},
                    {"users.name": "Bob", "orders.amount": 149.50},
                ],
                "query_type": "JOIN"
            }
            
            # TODO: C++ backend
            # result = cpp_join_engine.execute(query_string)
            
        elif "SELECT" in query_upper:
            # Hangi tablodan seçileceğini bul
            table_name = None
            for name in database.keys():
                if name.upper() in query_upper:
                    table_name = name
                    break
            
            if table_name:
                table = database[table_name]
                rows = table["rows"]
                
                # WHERE filtresi
                if "WHERE" in query_upper:
                    # Basit > filtresi
                    if "AGE > 25" in query_upper:
                        rows = [r for r in rows if r.get("age", 0) > 25]
                    elif "AGE >= 28" in query_upper:
                        rows = [r for r in rows if r.get("age", 0) >= 28]
                    result["query_type"] = "SELECT + WHERE"
                else:
                    result["query_type"] = "SELECT"
                
                result["columns"] = table["columns"]
                result["rows"] = rows
                
                # TODO: C++ backend
                # parsed_query = cpp_query_parser.parse(query_string)
                # result = cpp_query_engine.execute(parsed_query)
        
        execution_time = (time.time() - start_time) * 1000  # ms
        result["execution_time"] = round(execution_time, 2)
        result["success"] = True
        
        return jsonify(result)
        
    except Exception as e:
        return jsonify({
            "error": str(e),
            "success": False
        }), 400


@app.route('/api/benchmark/join', methods=['POST'])
def benchmark_join():
    """Nested Loop vs Hash Join karşılaştırması"""
    
    # Gerçek projede C++ fonksiyonlarınızı çağıracaksınız
    # nested_time = cpp_join_engine.benchmarkNestedLoop(table1, table2)
    # hash_time = cpp_join_engine.benchmarkHashJoin(table1, table2)
    
    # Şimdilik simülasyon
    nested_loop_time = random.uniform(30, 80)
    hash_join_time = random.uniform(5, 20)
    
    return jsonify({
        "nested_loop": {
            "time_ms": round(nested_loop_time, 2),
            "complexity": "O(N × M)",
            "memory_usage": "Low",
            "best_for": "Small tables, indexed columns"
        },
        "hash_join": {
            "time_ms": round(hash_join_time, 2),
            "complexity": "O(N + M)",
            "memory_usage": "High",
            "best_for": "Large tables, equality joins"
        }
    })


@app.route('/api/tables/<table_name>/search', methods=['POST'])
def search_by_index(table_name):
    """Index kullanarak arama yap"""
    if table_name not in database:
        return jsonify({"error": "Table not found"}), 404
    
    data = request.json
    column = data.get('column')
    value = data.get('value')
    search_type = data.get('type', 'exact')  # 'exact' or 'range'
    
    table = database[table_name]
    start_time = time.time()
    
    # Hangi index kullanılacak?
    index_used = None
    if column in table["indexes"]["hash"] and search_type == "exact":
        index_used = "Hash Index"
        # TODO: cpp_hash_index.search(value)
    elif column in table["indexes"]["btree"]:
        index_used = "B+ Tree Index"
        # TODO: cpp_btree_index.search(value) or cpp_btree_index.searchRange(min, max)
    else:
        index_used = "Full Table Scan"
    
    # Simüle edilmiş arama
    if search_type == "exact":
        results = [r for r in table["rows"] if str(r.get(column)) == str(value)]
    else:
        # Range search
        min_val = data.get('min_value', 0)
        max_val = data.get('max_value', 999999)
        results = [r for r in table["rows"] if min_val <= r.get(column, 0) <= max_val]
    
    execution_time = (time.time() - start_time) * 1000
    
    return jsonify({
        "results": results,
        "count": len(results),
        "index_used": index_used,
        "execution_time_ms": round(execution_time, 2)
    })


# ============================================================================
# HTML TEMPLATE (Inline for simplicity)
# ============================================================================

HTML_TEMPLATE = '''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>In-Memory Database Engine - CSE 211</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%);
            min-height: 100vh;
            color: #e2e8f0;
        }
        .header {
            background: linear-gradient(90deg, #1e293b 0%, #334155 50%, #1e293b 100%);
            border-bottom: 1px solid #334155;
            padding: 16px 24px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .header-left { display: flex; align-items: center; gap: 12px; }
        .logo {
            width: 40px; height: 40px;
            background: linear-gradient(135deg, #06b6d4 0%, #3b82f6 100%);
            border-radius: 10px;
            display: flex; align-items: center; justify-content: center;
            font-size: 20px;
        }
        .header-title h1 {
            font-size: 18px;
            background: linear-gradient(90deg, #06b6d4, #3b82f6);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .header-title p { font-size: 11px; color: #64748b; }
        .status {
            display: flex; align-items: center; gap: 8px;
            padding: 6px 12px;
            background: rgba(16, 185, 129, 0.1);
            border: 1px solid rgba(16, 185, 129, 0.3);
            border-radius: 20px;
        }
        .status-dot {
            width: 8px; height: 8px;
            background: #10b981;
            border-radius: 50%;
            animation: pulse 2s infinite;
        }
        @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
        .status span { font-size: 12px; color: #10b981; font-weight: 500; }
        .main-container { display: flex; height: calc(100vh - 73px); }
        .sidebar {
            width: 220px;
            background: rgba(15, 23, 42, 0.5);
            border-right: 1px solid #334155;
            display: flex; flex-direction: column;
        }
        .sidebar-header {
            padding: 16px;
            border-bottom: 1px solid #334155;
        }
        .sidebar-header span {
            font-size: 11px; font-weight: 600;
            color: #64748b; text-transform: uppercase;
            letter-spacing: 1px;
        }
        .sidebar-content { flex: 1; padding: 8px; overflow-y: auto; }
        .table-btn {
            width: 100%;
            display: flex; align-items: center; gap: 10px;
            padding: 10px 12px;
            border: 1px solid transparent;
            border-radius: 8px;
            background: transparent;
            color: #94a3b8;
            cursor: pointer;
            margin-bottom: 4px;
            text-align: left;
            transition: all 0.2s;
        }
        .table-btn:hover { background: rgba(51, 65, 85, 0.5); }
        .table-btn.active {
            background: rgba(6, 182, 212, 0.2);
            border-color: rgba(6, 182, 212, 0.3);
            color: #06b6d4;
        }
        .table-btn-icon { font-size: 14px; }
        .table-btn-info { flex: 1; }
        .table-btn-name { font-size: 13px; font-weight: 500; }
        .table-btn-rows { font-size: 11px; color: #64748b; }
        .sidebar-footer { padding: 12px; border-top: 1px solid #334155; }
        .new-table-btn {
            width: 100%; padding: 10px;
            background: linear-gradient(90deg, #06b6d4, #3b82f6);
            border: none; border-radius: 8px;
            color: white; font-weight: 500;
            cursor: pointer;
            display: flex; align-items: center; justify-content: center; gap: 8px;
        }
        .new-table-btn:hover { opacity: 0.9; }
        .main-content { flex: 1; display: flex; flex-direction: column; overflow: hidden; }
        .tabs {
            display: flex; gap: 4px;
            padding: 12px 16px 0;
            border-bottom: 1px solid #334155;
        }
        .tab {
            padding: 10px 16px;
            border: none; background: transparent;
            color: #64748b;
            font-size: 13px; font-weight: 500;
            cursor: pointer;
            border-bottom: 2px solid transparent;
            transition: all 0.2s;
        }
        .tab:hover { color: #94a3b8; }
        .tab.active {
            color: #06b6d4;
            border-bottom-color: #06b6d4;
            background: rgba(6, 182, 212, 0.05);
        }
        .tab-content { flex: 1; padding: 16px; overflow-y: auto; }
        .tab-panel { display: none; }
        .tab-panel.active { display: block; }
        .table-header {
            display: flex;
            justify-content: space-between;
            align-items: flex-start;
            margin-bottom: 16px;
        }
        .table-title {
            font-size: 22px; font-weight: 700;
            display: flex; align-items: center; gap: 10px;
        }
        .table-subtitle { font-size: 12px; color: #64748b; margin-top: 4px; }
        .table-subtitle span { color: #06b6d4; }
        .insert-btn {
            padding: 8px 16px;
            background: rgba(16, 185, 129, 0.2);
            border: 1px solid rgba(16, 185, 129, 0.3);
            border-radius: 8px;
            color: #10b981; font-weight: 500;
            cursor: pointer;
            display: flex; align-items: center; gap: 6px;
        }
        .insert-btn:hover { background: rgba(16, 185, 129, 0.3); }
        .schema-box {
            background: rgba(15, 23, 42, 0.5);
            border: 1px solid #334155;
            border-radius: 12px;
            padding: 16px;
            margin-bottom: 16px;
        }
        .schema-title {
            font-size: 11px; font-weight: 600;
            color: #64748b; text-transform: uppercase;
            letter-spacing: 1px; margin-bottom: 12px;
        }
        .schema-columns { display: flex; flex-wrap: wrap; gap: 8px; }
        .schema-column {
            display: flex; align-items: center; gap: 8px;
            padding: 8px 12px;
            background: rgba(51, 65, 85, 0.5);
            border: 1px solid #475569;
            border-radius: 8px;
        }
        .schema-column-name { font-family: monospace; font-size: 13px; }
        .schema-column-type {
            font-size: 11px; padding: 2px 8px;
            border-radius: 4px; font-weight: 500;
        }
        .type-int { background: rgba(59, 130, 246, 0.2); color: #60a5fa; }
        .type-string { background: rgba(168, 85, 247, 0.2); color: #c084fc; }
        .type-double { background: rgba(245, 158, 11, 0.2); color: #fbbf24; }
        .type-pk { background: rgba(6, 182, 212, 0.2); color: #06b6d4; }
        .data-table-container {
            background: rgba(15, 23, 42, 0.5);
            border: 1px solid #334155;
            border-radius: 12px;
            overflow: hidden;
        }
        .data-table { width: 100%; border-collapse: collapse; }
        .data-table th {
            background: rgba(51, 65, 85, 0.5);
            padding: 12px 16px;
            text-align: left;
            font-size: 11px; font-weight: 600;
            color: #64748b; text-transform: uppercase;
        }
        .data-table td {
            padding: 12px 16px;
            font-family: monospace; font-size: 13px;
            border-top: 1px solid #334155;
        }
        .data-table tr:hover { background: rgba(51, 65, 85, 0.3); }
        .delete-btn {
            padding: 6px;
            background: transparent; border: none;
            color: #ef4444; cursor: pointer;
            border-radius: 6px;
        }
        .delete-btn:hover { background: rgba(239, 68, 68, 0.2); }
        .query-grid { display: grid; grid-template-columns: 2fr 1fr; gap: 16px; }
        .query-box {
            background: rgba(15, 23, 42, 0.5);
            border: 1px solid #334155;
            border-radius: 12px;
            overflow: hidden;
        }
        .query-box-header {
            padding: 12px 16px;
            background: rgba(51, 65, 85, 0.5);
            border-bottom: 1px solid #334155;
            font-size: 13px; font-weight: 500;
            color: #94a3b8;
        }
        .query-box-content { padding: 16px; }
        .query-textarea {
            width: 100%; height: 120px;
            background: #0f172a;
            border: 1px solid #475569;
            border-radius: 8px;
            padding: 12px;
            color: #e2e8f0;
            font-family: monospace; font-size: 13px;
            resize: none; outline: none;
        }
        .query-textarea:focus { border-color: #06b6d4; }
        .query-actions {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-top: 12px;
        }
        .query-hint { font-size: 11px; color: #64748b; }
        .execute-btn {
            padding: 10px 20px;
            background: linear-gradient(90deg, #06b6d4, #3b82f6);
            border: none; border-radius: 8px;
            color: white; font-weight: 500;
            cursor: pointer;
            display: flex; align-items: center; gap: 8px;
        }
        .execute-btn:hover { opacity: 0.9; }
        .sample-queries { margin-top: 16px; }
        .sample-query {
            width: 100%;
            padding: 10px 12px;
            background: rgba(51, 65, 85, 0.3);
            border: none; border-radius: 8px;
            color: #94a3b8;
            font-family: monospace; font-size: 12px;
            text-align: left; cursor: pointer;
            margin-bottom: 8px;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }
        .sample-query:hover { background: rgba(51, 65, 85, 0.5); color: #e2e8f0; }
        .history-list { max-height: 300px; overflow-y: auto; }
        .history-item {
            padding: 8px 12px;
            background: rgba(51, 65, 85, 0.3);
            border-radius: 6px;
            margin-bottom: 6px;
            font-family: monospace; font-size: 11px;
            color: #94a3b8; cursor: pointer;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }
        .history-item:hover { background: rgba(51, 65, 85, 0.5); }
        .results-box { margin-top: 16px; }
        .results-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 12px 16px;
            background: rgba(51, 65, 85, 0.5);
            border-bottom: 1px solid #334155;
        }
        .results-info { display: flex; align-items: center; gap: 12px; }
        .results-badge {
            padding: 4px 10px;
            background: rgba(6, 182, 212, 0.2);
            color: #06b6d4;
            border-radius: 4px;
            font-size: 11px; font-weight: 500;
        }
        .results-meta { font-size: 12px; color: #64748b; }
        .index-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; margin-bottom: 16px; }
        .index-card { border-radius: 12px; padding: 20px; border: 1px solid; }
        .index-card.btree {
            background: linear-gradient(135deg, rgba(59, 130, 246, 0.1), rgba(6, 182, 212, 0.1));
            border-color: rgba(59, 130, 246, 0.3);
        }
        .index-card.hash {
            background: linear-gradient(135deg, rgba(168, 85, 247, 0.1), rgba(236, 72, 153, 0.1));
            border-color: rgba(168, 85, 247, 0.3);
        }
        .index-card-header { display: flex; align-items: center; gap: 12px; margin-bottom: 16px; }
        .index-card-icon {
            width: 40px; height: 40px;
            border-radius: 10px;
            display: flex; align-items: center; justify-content: center;
            font-size: 20px;
        }
        .index-card.btree .index-card-icon { background: rgba(59, 130, 246, 0.2); }
        .index-card.hash .index-card-icon { background: rgba(168, 85, 247, 0.2); }
        .index-card-title { font-weight: 600; font-size: 15px; }
        .index-card-subtitle { font-size: 12px; color: #64748b; }
        .index-columns { margin-bottom: 16px; }
        .index-columns-title {
            font-size: 11px; color: #64748b;
            text-transform: uppercase;
            letter-spacing: 0.5px; margin-bottom: 8px;
        }
        .index-column-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 8px 12px;
            background: rgba(51, 65, 85, 0.5);
            border-radius: 6px;
            margin-bottom: 6px;
        }
        .index-column-name { font-family: monospace; font-size: 13px; }
        .index-column-status { font-size: 11px; }
        .index-card.btree .index-column-status { color: #60a5fa; }
        .index-card.hash .index-column-status { color: #c084fc; }
        .index-stats { padding-top: 16px; border-top: 1px solid rgba(255, 255, 255, 0.1); }
        .index-stat { display: flex; justify-content: space-between; font-size: 12px; margin-bottom: 6px; }
        .index-stat-label { color: #64748b; }
        .index-stat-value { font-family: monospace; }
        .tree-viz {
            background: rgba(15, 23, 42, 0.5);
            border: 1px solid #334155;
            border-radius: 12px;
            padding: 20px;
        }
        .tree-viz-title {
            font-size: 11px; font-weight: 600;
            color: #64748b; text-transform: uppercase;
            letter-spacing: 1px; margin-bottom: 16px;
        }
        .tree-container { display: flex; flex-direction: column; align-items: center; }
        .tree-node {
            padding: 8px 20px;
            border-radius: 8px;
            font-family: monospace; font-size: 13px;
        }
        .tree-node.root {
            background: rgba(6, 182, 212, 0.2);
            border: 1px solid rgba(6, 182, 212, 0.5);
            color: #06b6d4;
        }
        .tree-node.leaf {
            background: rgba(59, 130, 246, 0.2);
            border: 1px solid rgba(59, 130, 246, 0.5);
            color: #60a5fa;
        }
        .tree-line { width: 2px; height: 24px; background: #475569; }
        .tree-level { display: flex; gap: 60px; }
        .tree-leaf-container { display: flex; flex-direction: column; align-items: center; }
        .tree-leaf-label { font-size: 11px; color: #64748b; margin-top: 4px; }
        .tree-link-info { margin-top: 16px; font-size: 12px; color: #64748b; }
        .tree-link-info span { font-family: monospace; color: #94a3b8; }
        .perf-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 16px;
        }
        .perf-title { font-size: 20px; font-weight: 700; }
        .benchmark-btn {
            padding: 10px 20px;
            background: linear-gradient(90deg, #f59e0b, #f97316);
            border: none; border-radius: 8px;
            color: white; font-weight: 500;
            cursor: pointer;
            display: flex; align-items: center; gap: 8px;
        }
        .benchmark-btn:hover { opacity: 0.9; }
        .perf-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; margin-bottom: 16px; }
        .perf-card { border-radius: 12px; padding: 20px; border: 1px solid; }
        .perf-card.nested {
            background: linear-gradient(135deg, rgba(249, 115, 22, 0.1), rgba(239, 68, 68, 0.1));
            border-color: rgba(249, 115, 22, 0.3);
        }
        .perf-card.hash {
            background: linear-gradient(135deg, rgba(16, 185, 129, 0.1), rgba(20, 184, 166, 0.1));
            border-color: rgba(16, 185, 129, 0.3);
        }
        .perf-card-title { font-size: 16px; font-weight: 600; margin-bottom: 4px; }
        .perf-card-subtitle { font-size: 12px; color: #64748b; margin-bottom: 16px; }
        .perf-value { text-align: center; padding: 20px 0; }
        .perf-number { font-size: 42px; font-weight: 700; }
        .perf-card.nested .perf-number { color: #fb923c; }
        .perf-card.hash .perf-number { color: #34d399; }
        .perf-unit { font-size: 14px; color: #64748b; }
        .perf-stats { padding-top: 16px; border-top: 1px solid rgba(255, 255, 255, 0.1); }
        .perf-stat { display: flex; justify-content: space-between; font-size: 13px; margin-bottom: 8px; }
        .perf-stat-label { color: #64748b; }
        .perf-stat-value { font-family: monospace; }
        .perf-stat-value.good { color: #34d399; }
        .perf-stat-value.warn { color: #fbbf24; }
        .guide-box {
            background: rgba(15, 23, 42, 0.5);
            border: 1px solid #334155;
            border-radius: 12px;
            padding: 20px;
        }
        .guide-title {
            font-size: 11px; font-weight: 600;
            color: #64748b; text-transform: uppercase;
            letter-spacing: 1px; margin-bottom: 16px;
        }
        .guide-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; }
        .guide-card { padding: 16px; background: rgba(51, 65, 85, 0.3); border-radius: 8px; }
        .guide-card h4 { font-size: 14px; margin-bottom: 10px; }
        .guide-card ul { list-style: none; }
        .guide-card li {
            font-size: 13px; color: #94a3b8;
            margin-bottom: 6px; padding-left: 16px;
            position: relative;
        }
        .guide-card li::before {
            content: "•"; position: absolute;
            left: 0; color: #64748b;
        }
        .modal-overlay {
            position: fixed; inset: 0;
            background: rgba(0, 0, 0, 0.6);
            display: none;
            align-items: center; justify-content: center;
            z-index: 1000;
        }
        .modal-overlay.active { display: flex; }
        .modal {
            background: #1e293b;
            border: 1px solid #475569;
            border-radius: 16px;
            width: 100%; max-width: 480px;
            padding: 24px;
        }
        .modal-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 20px;
        }
        .modal-title { font-size: 18px; font-weight: 700; }
        .modal-close {
            width: 32px; height: 32px;
            background: transparent; border: none;
            color: #64748b; cursor: pointer;
            border-radius: 8px; font-size: 20px;
        }
        .modal-close:hover { background: rgba(51, 65, 85, 0.5); }
        .form-group { margin-bottom: 16px; }
        .form-label {
            display: block;
            font-size: 13px; font-weight: 500;
            color: #94a3b8; margin-bottom: 8px;
        }
        .form-input {
            width: 100%;
            padding: 10px 14px;
            background: #0f172a;
            border: 1px solid #475569;
            border-radius: 8px;
            color: #e2e8f0;
            font-size: 14px; outline: none;
        }
        .form-input:focus { border-color: #06b6d4; }
        .form-select {
            width: 100%;
            padding: 10px 14px;
            background: #0f172a;
            border: 1px solid #475569;
            border-radius: 8px;
            color: #e2e8f0;
            font-size: 14px; outline: none;
        }
        .modal-footer {
            display: flex;
            justify-content: flex-end;
            gap: 12px;
            margin-top: 24px;
            padding-top: 16px;
            border-top: 1px solid #334155;
        }
        .btn-cancel {
            padding: 10px 20px;
            background: transparent; border: none;
            color: #64748b; cursor: pointer;
        }
        .btn-cancel:hover { color: #94a3b8; }
        .btn-primary {
            padding: 10px 20px;
            background: linear-gradient(90deg, #06b6d4, #3b82f6);
            border: none; border-radius: 8px;
            color: white; font-weight: 500;
            cursor: pointer;
        }
        .btn-success {
            padding: 10px 20px;
            background: linear-gradient(90deg, #10b981, #14b8a6);
            border: none; border-radius: 8px;
            color: white; font-weight: 500;
            cursor: pointer;
        }
        .notification {
            position: fixed;
            top: 80px; right: 20px;
            padding: 12px 20px;
            background: #10b981;
            border-radius: 8px;
            color: white; font-weight: 500;
            display: none;
            align-items: center; gap: 8px;
            z-index: 1001;
            animation: slideIn 0.3s ease;
        }
        .notification.active { display: flex; }
        @keyframes slideIn {
            from { transform: translateX(100%); opacity: 0; }
            to { transform: translateX(0); opacity: 1; }
        }
        .empty-state { text-align: center; padding: 40px; color: #64748b; }
        .index-type-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
        .index-type-btn {
            padding: 16px;
            background: rgba(51, 65, 85, 0.5);
            border: 2px solid #475569;
            border-radius: 12px;
            cursor: pointer; text-align: center;
        }
        .index-type-btn:hover { border-color: #64748b; }
        .index-type-btn.active.btree {
            background: rgba(59, 130, 246, 0.2);
            border-color: rgba(59, 130, 246, 0.5);
        }
        .index-type-btn.active.hash {
            background: rgba(168, 85, 247, 0.2);
            border-color: rgba(168, 85, 247, 0.5);
        }
        .index-type-icon { font-size: 28px; margin-bottom: 8px; }
        .index-type-name { font-weight: 600; font-size: 14px; margin-bottom: 4px; }
        .index-type-desc { font-size: 11px; color: #64748b; }
        .create-index-btn {
            padding: 8px 16px;
            background: linear-gradient(90deg, #a855f7, #ec4899);
            border: none; border-radius: 8px;
            color: white; font-weight: 500;
            cursor: pointer;
            display: flex; align-items: center; gap: 6px;
        }
        .api-info {
            background: rgba(6, 182, 212, 0.1);
            border: 1px solid rgba(6, 182, 212, 0.3);
            border-radius: 8px;
            padding: 12px;
            margin-bottom: 16px;
            font-size: 12px;
            color: #06b6d4;
        }
    </style>
</head>
<body>
    <header class="header">
        <div class="header-left">
            <div class="logo">🗄️</div>
            <div class="header-title">
                <h1>In-Memory Database Engine</h1>
                <p>CSE 211 - Data Structures Project (Python API)</p>
            </div>
        </div>
        <div class="status">
            <div class="status-dot"></div>
            <span>API Connected</span>
        </div>
    </header>

    <div class="main-container">
        <aside class="sidebar">
            <div class="sidebar-header">
                <span>Tables</span>
            </div>
            <div class="sidebar-content" id="tableList"></div>
            <div class="sidebar-footer">
                <button class="new-table-btn" onclick="openCreateTableModal()">
                    ➕ New Table
                </button>
            </div>
        </aside>

        <main class="main-content">
            <div class="tabs">
                <button class="tab active" data-tab="tables" onclick="switchTab('tables')">📊 Table View</button>
                <button class="tab" data-tab="query" onclick="switchTab('query')">💻 Query Editor</button>
                <button class="tab" data-tab="indexes" onclick="switchTab('indexes')">🌳 Indexes</button>
                <button class="tab" data-tab="performance" onclick="switchTab('performance')">⚡ Performance</button>
            </div>

            <div class="tab-content">
                <div id="tables-panel" class="tab-panel active">
                    <div id="tableView"></div>
                </div>

                <div id="query-panel" class="tab-panel">
                    <div class="api-info">
                        🔗 API Endpoint: POST /api/query - SQL sorguları buradan çalıştırılır
                    </div>
                    <div class="query-grid">
                        <div>
                            <div class="query-box">
                                <div class="query-box-header">SQL Query</div>
                                <div class="query-box-content">
                                    <textarea id="queryInput" class="query-textarea" placeholder="Enter your SQL query here..."></textarea>
                                    <div class="query-actions">
                                        <span class="query-hint">Supports SELECT, WHERE, JOIN, ORDER BY</span>
                                        <button class="execute-btn" onclick="executeQuery()">
                                            ▶️ Execute
                                        </button>
                                    </div>
                                </div>
                            </div>
                            <div class="sample-queries">
                                <div class="schema-title">Sample Queries</div>
                                <button class="sample-query" onclick="setQuery('SELECT name, age FROM users WHERE age > 25')">SELECT name, age FROM users WHERE age > 25</button>
                                <button class="sample-query" onclick="setQuery('SELECT * FROM users')">SELECT * FROM users</button>
                                <button class="sample-query" onclick="setQuery('SELECT users.name, orders.amount FROM users JOIN orders ON users.id = orders.user_id')">SELECT users.name, orders.amount FROM users JOIN orders...</button>
                            </div>
                        </div>
                        <div>
                            <div class="query-box">
                                <div class="query-box-header">🕐 History</div>
                                <div class="query-box-content">
                                    <div id="queryHistory" class="history-list">
                                        <p class="empty-state">No queries yet</p>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div id="queryResults" class="results-box" style="display: none;"></div>
                </div>

                <div id="indexes-panel" class="tab-panel">
                    <div class="table-header">
                        <h2 class="table-title">Index Management</h2>
                        <button class="create-index-btn" onclick="openCreateIndexModal()">
                            ➕ Create Index
                        </button>
                    </div>
                    <div id="indexView"></div>
                </div>

                <div id="performance-panel" class="tab-panel">
                    <div class="perf-header">
                        <h2 class="perf-title">Join Performance Comparison</h2>
                        <button class="benchmark-btn" onclick="runBenchmark()">
                            ⚡ Run Benchmark
                        </button>
                    </div>
                    <div id="perfResults">
                        <div class="empty-state">Click "Run Benchmark" to compare join algorithms</div>
                    </div>
                    <div class="guide-box" style="margin-top: 16px;">
                        <div class="guide-title">Algorithm Selection Guide</div>
                        <div class="guide-grid">
                            <div class="guide-card">
                                <h4>Use Nested Loop When:</h4>
                                <ul>
                                    <li>One table is very small (&lt;100 rows)</li>
                                    <li>Join column is indexed</li>
                                    <li>Memory is constrained</li>
                                </ul>
                            </div>
                            <div class="guide-card">
                                <h4>Use Hash Join When:</h4>
                                <ul>
                                    <li>Both tables are large</li>
                                    <li>Equality join condition</li>
                                    <li>Sufficient memory available</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </main>
    </div>

    <!-- Modals -->
    <div id="createTableModal" class="modal-overlay">
        <div class="modal">
            <div class="modal-header">
                <h3 class="modal-title">Create New Table</h3>
                <button class="modal-close" onclick="closeModal('createTableModal')">×</button>
            </div>
            <div class="form-group">
                <label class="form-label">Table Name</label>
                <input type="text" id="newTableName" class="form-input" placeholder="e.g., products">
            </div>
            <div class="form-group">
                <label class="form-label">Columns</label>
                <div id="columnInputs"></div>
                <button style="margin-top: 8px; background: transparent; border: none; color: #06b6d4; cursor: pointer;" onclick="addColumnInput()">
                    ➕ Add Column
                </button>
            </div>
            <div class="modal-footer">
                <button class="btn-cancel" onclick="closeModal('createTableModal')">Cancel</button>
                <button class="btn-primary" onclick="createTable()">Create Table</button>
            </div>
        </div>
    </div>

    <div id="insertRowModal" class="modal-overlay">
        <div class="modal">
            <div class="modal-header">
                <h3 class="modal-title">Insert Row</h3>
                <button class="modal-close" onclick="closeModal('insertRowModal')">×</button>
            </div>
            <div id="rowInputs"></div>
            <div class="modal-footer">
                <button class="btn-cancel" onclick="closeModal('insertRowModal')">Cancel</button>
                <button class="btn-success" onclick="insertRow()">Insert Row</button>
            </div>
        </div>
    </div>

    <div id="createIndexModal" class="modal-overlay">
        <div class="modal">
            <div class="modal-header">
                <h3 class="modal-title">Create Index</h3>
                <button class="modal-close" onclick="closeModal('createIndexModal')">×</button>
            </div>
            <div class="form-group">
                <label class="form-label">Column</label>
                <select id="indexColumn" class="form-select">
                    <option value="">Select column...</option>
                </select>
            </div>
            <div class="form-group">
                <label class="form-label">Index Type</label>
                <div class="index-type-grid">
                    <div class="index-type-btn btree active" onclick="selectIndexType('btree')">
                        <div class="index-type-icon">🌳</div>
                        <div class="index-type-name">B+ Tree</div>
                        <div class="index-type-desc">Range queries</div>
                    </div>
                    <div class="index-type-btn hash" onclick="selectIndexType('hash')">
                        <div class="index-type-icon">#️⃣</div>
                        <div class="index-type-name">Hash</div>
                        <div class="index-type-desc">Exact matches</div>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button class="btn-cancel" onclick="closeModal('createIndexModal')">Cancel</button>
                <button class="btn-primary" onclick="createIndex()">Create Index</button>
            </div>
        </div>
    </div>

    <div id="notification" class="notification">
        <span>✓</span>
        <span id="notificationText">Success!</span>
    </div>

    <script>
        const API_BASE = '';
        let selectedTable = 'users';
        let selectedIndexType = 'btree';
        let queryHistory = [];
        let tablesData = {};

        document.addEventListener('DOMContentLoaded', () => {
            loadTables();
        });

        // API Functions
        async function loadTables() {
            const res = await fetch(`${API_BASE}/api/tables`);
            tablesData = await res.json();
            renderTableList();
            loadTableData(selectedTable);
        }

        async function loadTableData(tableName) {
            const res = await fetch(`${API_BASE}/api/tables/${tableName}`);
            const table = await res.json();
            tablesData[tableName] = table;
            renderTableView();
            renderIndexView();
        }

        // Render Functions
        function renderTableList() {
            const container = document.getElementById('tableList');
            container.innerHTML = Object.entries(tablesData).map(([name, table]) => `
                <button class="table-btn ${selectedTable === name ? 'active' : ''}" onclick="selectTable('${name}')">
                    <span class="table-btn-icon">📋</span>
                    <div class="table-btn-info">
                        <div class="table-btn-name">${name}</div>
                        <div class="table-btn-rows">${table.row_count || table.rows?.length || 0} rows</div>
                    </div>
                </button>
            `).join('');
        }

        function renderTableView() {
            const table = tablesData[selectedTable];
            if (!table || !table.rows) return;
            
            const container = document.getElementById('tableView');
            container.innerHTML = `
                <div class="table-header">
                    <div>
                        <h2 class="table-title">📊 ${table.name}</h2>
                        <p class="table-subtitle">
                            ${table.rows.length} rows • ${table.columns.length} columns • 
                            PK: <span>${table.primary_key}</span>
                        </p>
                    </div>
                    <button class="insert-btn" onclick="openInsertRowModal()">
                        ➕ Insert Row
                    </button>
                </div>
                <div class="schema-box">
                    <div class="schema-title">Schema</div>
                    <div class="schema-columns">
                        ${table.columns.map((col, idx) => `
                            <div class="schema-column">
                                <span class="schema-column-name">${col}</span>
                                <span class="schema-column-type type-${table.types[idx].toLowerCase()}">${table.types[idx]}</span>
                                ${col === table.primary_key ? '<span class="schema-column-type type-pk">PK</span>' : ''}
                            </div>
                        `).join('')}
                    </div>
                </div>
                <div class="data-table-container">
                    <table class="data-table">
                        <thead>
                            <tr>
                                ${table.columns.map(col => `<th>${col}</th>`).join('')}
                                <th style="text-align: right;">Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            ${table.rows.map((row, idx) => `
                                <tr>
                                    ${table.columns.map(col => `<td>${row[col]}</td>`).join('')}
                                    <td style="text-align: right;">
                                        <button class="delete-btn" onclick="deleteRow(${idx})">🗑️</button>
                                    </td>
                                </tr>
                            `).join('')}
                        </tbody>
                    </table>
                </div>
            `;
        }

        function renderIndexView() {
            const table = tablesData[selectedTable];
            if (!table) return;
            
            const container = document.getElementById('indexView');
            container.innerHTML = `
                <div class="index-grid">
                    <div class="index-card btree">
                        <div class="index-card-header">
                            <div class="index-card-icon">🌳</div>
                            <div>
                                <div class="index-card-title">B+ Tree Index</div>
                                <div class="index-card-subtitle">Optimized for range queries</div>
                            </div>
                        </div>
                        <div class="index-columns">
                            <div class="index-columns-title">Indexed Columns</div>
                            ${(table.indexes?.btree || []).length === 0 ? 
                                '<p style="color: #64748b; font-size: 13px;">No B+ Tree indexes</p>' :
                                (table.indexes?.btree || []).map(col => `
                                    <div class="index-column-item">
                                        <span class="index-column-name">${col}</span>
                                        <span class="index-column-status">Active</span>
                                    </div>
                                `).join('')
                            }
                        </div>
                        <div class="index-stats">
                            <div class="index-stat">
                                <span class="index-stat-label">Complexity:</span>
                                <span class="index-stat-value">O(log n)</span>
                            </div>
                            <div class="index-stat">
                                <span class="index-stat-label">Best for:</span>
                                <span class="index-stat-value">Range scans</span>
                            </div>
                        </div>
                    </div>
                    <div class="index-card hash">
                        <div class="index-card-header">
                            <div class="index-card-icon">#️⃣</div>
                            <div>
                                <div class="index-card-title">Hash Index</div>
                                <div class="index-card-subtitle">Optimized for equality lookups</div>
                            </div>
                        </div>
                        <div class="index-columns">
                            <div class="index-columns-title">Indexed Columns</div>
                            ${(table.indexes?.hash || []).length === 0 ? 
                                '<p style="color: #64748b; font-size: 13px;">No Hash indexes</p>' :
                                (table.indexes?.hash || []).map(col => `
                                    <div class="index-column-item">
                                        <span class="index-column-name">${col}</span>
                                        <span class="index-column-status">Active</span>
                                    </div>
                                `).join('')
                            }
                        </div>
                        <div class="index-stats">
                            <div class="index-stat">
                                <span class="index-stat-label">Complexity:</span>
                                <span class="index-stat-value">O(1) avg</span>
                            </div>
                            <div class="index-stat">
                                <span class="index-stat-label">Best for:</span>
                                <span class="index-stat-value">Exact matches</span>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="tree-viz">
                    <div class="tree-viz-title">B+ Tree Structure (${table.primary_key} Index)</div>
                    <div class="tree-container">
                        <div class="tree-node root">[2]</div>
                        <div class="tree-line"></div>
                        <div class="tree-level">
                            <div class="tree-leaf-container">
                                <div class="tree-node leaf">[1, 2]</div>
                                <div class="tree-leaf-label">Leaf</div>
                            </div>
                            <div class="tree-leaf-container">
                                <div class="tree-node leaf">[3]</div>
                                <div class="tree-leaf-label">Leaf</div>
                            </div>
                        </div>
                        <div class="tree-link-info">Leaf nodes linked: <span>[1,2] → [3]</span></div>
                    </div>
                </div>
            `;
        }

        // Tab Switching
        function switchTab(tabId) {
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
            document.querySelector(`[data-tab="${tabId}"]`).classList.add('active');
            document.getElementById(`${tabId}-panel`).classList.add('active');
        }

        function selectTable(name) {
            selectedTable = name;
            loadTableData(name);
            renderTableList();
        }

        // Query Functions
        function setQuery(q) {
            document.getElementById('queryInput').value = q;
        }

        async function executeQuery() {
            const query = document.getElementById('queryInput').value.trim();
            if (!query) return;

            const res = await fetch(`${API_BASE}/api/query`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ query })
            });
            const result = await res.json();

            queryHistory.unshift(query);
            queryHistory = queryHistory.slice(0, 10);
            renderQueryHistory();

            const resultsContainer = document.getElementById('queryResults');
            resultsContainer.style.display = 'block';
            resultsContainer.innerHTML = `
                <div class="query-box">
                    <div class="results-header">
                        <div class="results-info">
                            <span style="font-weight: 500;">Results</span>
                            <span class="results-badge">${result.query_type}</span>
                        </div>
                        <div class="results-meta">
                            ${result.rows?.length || 0} rows • ${result.execution_time}ms
                        </div>
                    </div>
                    <table class="data-table">
                        <thead>
                            <tr>${(result.columns || []).map(col => `<th>${col}</th>`).join('')}</tr>
                        </thead>
                        <tbody>
                            ${(result.rows || []).map(row => `
                                <tr>${(result.columns || []).map(col => `<td>${row[col]}</td>`).join('')}</tr>
                            `).join('')}
                        </tbody>
                    </table>
                </div>
            `;
            showNotification('Query executed successfully');
        }

        function renderQueryHistory() {
            const container = document.getElementById('queryHistory');
            if (queryHistory.length === 0) {
                container.innerHTML = '<p class="empty-state">No queries yet</p>';
                return;
            }
            container.innerHTML = queryHistory.map(q => `
                <div class="history-item" onclick="setQuery('${q.replace(/'/g, "\\'")}')">${q}</div>
            `).join('');
        }

        // Benchmark
        async function runBenchmark() {
            const res = await fetch(`${API_BASE}/api/benchmark/join`, { method: 'POST' });
            const data = await res.json();

            document.getElementById('perfResults').innerHTML = `
                <div class="perf-grid">
                    <div class="perf-card nested">
                        <div class="perf-card-title">Nested Loop Join</div>
                        <div class="perf-card-subtitle">Baseline algorithm</div>
                        <div class="perf-value">
                            <div class="perf-number">${data.nested_loop.time_ms}</div>
                            <div class="perf-unit">milliseconds</div>
                        </div>
                        <div class="perf-stats">
                            <div class="perf-stat">
                                <span class="perf-stat-label">Complexity</span>
                                <span class="perf-stat-value">${data.nested_loop.complexity}</span>
                            </div>
                            <div class="perf-stat">
                                <span class="perf-stat-label">Memory</span>
                                <span class="perf-stat-value good">${data.nested_loop.memory_usage}</span>
                            </div>
                        </div>
                    </div>
                    <div class="perf-card hash">
                        <div class="perf-card-title">Hash Join</div>
                        <div class="perf-card-subtitle">Optimized algorithm</div>
                        <div class="perf-value">
                            <div class="perf-number">${data.hash_join.time_ms}</div>
                            <div class="perf-unit">milliseconds</div>
                        </div>
                        <div class="perf-stats">
                            <div class="perf-stat">
                                <span class="perf-stat-label">Complexity</span>
                                <span class="perf-stat-value">${data.hash_join.complexity}</span>
                            </div>
                            <div class="perf-stat">
                                <span class="perf-stat-label">Memory</span>
                                <span class="perf-stat-value warn">${data.hash_join.memory_usage}</span>
                            </div>
                        </div>
                    </div>
                </div>
            `;
            showNotification('Benchmark completed');
        }

        // Modal Functions
        function openCreateTableModal() {
            initColumnInputs();
            document.getElementById('newTableName').value = '';
            document.getElementById('createTableModal').classList.add('active');
        }

        function openInsertRowModal() {
            const table = tablesData[selectedTable];
            const container = document.getElementById('rowInputs');
            container.innerHTML = table.columns.map((col, idx) => `
                <div class="form-group">
                    <label class="form-label">${col} <span style="color: #64748b; font-size: 11px;">(${table.types[idx]})</span></label>
                    <input type="${table.types[idx] === 'STRING' ? 'text' : 'number'}" 
                           id="row_${col}" class="form-input" placeholder="Enter ${col}"
                           ${table.types[idx] === 'DOUBLE' ? 'step="0.01"' : ''}>
                </div>
            `).join('');
            document.getElementById('insertRowModal').classList.add('active');
        }

        function openCreateIndexModal() {
            const table = tablesData[selectedTable];
            const select = document.getElementById('indexColumn');
            select.innerHTML = '<option value="">Select column...</option>' +
                table.columns.map(col => `<option value="${col}">${col}</option>`).join('');
            selectedIndexType = 'btree';
            document.querySelectorAll('.index-type-btn').forEach(b => b.classList.remove('active'));
            document.querySelector('.index-type-btn.btree').classList.add('active');
            document.getElementById('createIndexModal').classList.add('active');
        }

        function closeModal(id) {
            document.getElementById(id).classList.remove('active');
        }

        function initColumnInputs() {
            const container = document.getElementById('columnInputs');
            container.innerHTML = `
                <div style="display: flex; gap: 8px; margin-bottom: 8px;">
                    <input type="text" class="form-input col-name" placeholder="Column name" style="flex: 1;">
                    <select class="form-select col-type" style="width: 120px;">
                        <option value="INT">INT</option>
                        <option value="STRING">STRING</option>
                        <option value="DOUBLE">DOUBLE</option>
                    </select>
                </div>
            `;
        }

        function addColumnInput() {
            const container = document.getElementById('columnInputs');
            const div = document.createElement('div');
            div.style.cssText = 'display: flex; gap: 8px; margin-bottom: 8px;';
            div.innerHTML = `
                <input type="text" class="form-input col-name" placeholder="Column name" style="flex: 1;">
                <select class="form-select col-type" style="width: 120px;">
                    <option value="INT">INT</option>
                    <option value="STRING">STRING</option>
                    <option value="DOUBLE">DOUBLE</option>
                </select>
            `;
            container.appendChild(div);
        }

        function selectIndexType(type) {
            selectedIndexType = type;
            document.querySelectorAll('.index-type-btn').forEach(b => b.classList.remove('active'));
            document.querySelector(`.index-type-btn.${type}`).classList.add('active');
        }

        // CRUD Operations
        async function createTable() {
            const name = document.getElementById('newTableName').value.trim();
            if (!name) { showNotification('Please enter table name'); return; }

            const colNames = Array.from(document.querySelectorAll('.col-name')).map(i => i.value.trim());
            const colTypes = Array.from(document.querySelectorAll('.col-type')).map(s => s.value);

            if (colNames.some(n => !n)) { showNotification('Please fill all column names'); return; }

            await fetch(`${API_BASE}/api/tables`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ name, columns: colNames, types: colTypes })
            });

            closeModal('createTableModal');
            await loadTables();
            selectTable(name);
            showNotification(`Table "${name}" created`);
        }

        async function insertRow() {
            const table = tablesData[selectedTable];
            const row = {};
            table.columns.forEach((col, idx) => {
                const input = document.getElementById(`row_${col}`);
                let value = input.value;
                if (table.types[idx] === 'INT') value = parseInt(value) || 0;
                else if (table.types[idx] === 'DOUBLE') value = parseFloat(value) || 0;
                row[col] = value;
            });

            await fetch(`${API_BASE}/api/tables/${selectedTable}/rows`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(row)
            });

            closeModal('insertRowModal');
            await loadTableData(selectedTable);
            showNotification('Row inserted successfully');
        }

        async function deleteRow(index) {
            await fetch(`${API_BASE}/api/tables/${selectedTable}/rows/${index}`, { method: 'DELETE' });
            await loadTableData(selectedTable);
            showNotification('Row deleted');
        }

        async function createIndex() {
            const column = document.getElementById('indexColumn').value;
            if (!column) { showNotification('Please select a column'); return; }

            await fetch(`${API_BASE}/api/tables/${selectedTable}/indexes`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ column, type: selectedIndexType })
            });

            closeModal('createIndexModal');
            await loadTableData(selectedTable);
            showNotification(`${selectedIndexType.toUpperCase()} index created`);
        }

        function showNotification(message) {
            const notif = document.getElementById('notification');
            document.getElementById('notificationText').textContent = message;
            notif.classList.add('active');
            setTimeout(() => notif.classList.remove('active'), 3000);
        }
    </script>
</body>
</html>
'''


# ============================================================================
# RUN SERVER
# ============================================================================

if __name__ == '__main__':
    print("""
    ╔═══════════════════════════════════════════════════════════╗
    ║     In-Memory Database Engine - Python API                ║
    ║     CSE 211 - Data Structures Project                     ║
    ╠═══════════════════════════════════════════════════════════╣
    ║                                                           ║
    ║     🌐 GUI:  http://localhost:5000                        ║
    ║     📡 API:  http://localhost:5000/api/                   ║
    ║                                                           ║
    ║     API Endpoints:                                        ║
    ║       GET  /api/tables          - List all tables         ║
    ║       GET  /api/tables/<name>   - Get table details       ║
    ║       POST /api/tables          - Create table            ║
    ║       POST /api/tables/<n>/rows - Insert row              ║
    ║       DELETE /api/tables/<n>/rows/<i> - Delete row        ║
    ║       POST /api/query           - Execute SQL query       ║
    ║       POST /api/benchmark/join  - Run join benchmark      ║
    ║                                                           ║
    ║     Press Ctrl+C to stop the server                       ║
    ╚═══════════════════════════════════════════════════════════╝
    """)
    app.run(debug=True, host='0.0.0.0', port=5000)
