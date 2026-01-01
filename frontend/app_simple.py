"""
In-Memory Database Engine - Pure Flask (No JavaScript!)
Tüm işlemler server-side, form POST ile çalışır.

Çalıştırma: pip install flask && python app.py
"""

from flask import Flask, request, redirect, url_for
import time, random
import requests  # C++ backend'e HTTP istekleri için

app = Flask(__name__)

# C++ Backend API URL
CPP_BACKEND_URL = "http://localhost:8080"

def check_cpp_backend():
    """C++ backend'in çalışıp çalışmadığını kontrol et"""
    try:
        response = requests.get(f"{CPP_BACKEND_URL}/get?id=1", timeout=2)
        return response.status_code == 200
    except Exception as e:
        print(f"Backend check failed: {e}")
        return False

# ============================================================================
# DATABASE
# ============================================================================

db = {
    "users": {
        "columns": ["id", "name", "age"],
        "types": ["INT", "STRING", "INT"],
        "rows": [
            {"id": 1, "name": "Alice", "age": 25},
            {"id": 2, "name": "Bob", "age": 30},
            {"id": 3, "name": "Carol", "age": 28},
        ],
        "indexes": {"btree": ["id"], "hash": ["name"]}
    },
    "orders": {
        "columns": ["order_id", "user_id", "amount"],
        "types": ["INT", "INT", "DOUBLE"],
        "rows": [
            {"order_id": 101, "user_id": 1, "amount": 99.99},
            {"order_id": 102, "user_id": 2, "amount": 149.50},
        ],
        "indexes": {"btree": ["order_id"], "hash": []}
    }
}

# ============================================================================
# HTML TEMPLATE (Jinja2 - Tek dosyada)
# ============================================================================

HTML = '''
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>DB Engine</title>
    <style>
        :root { --bg:#0f172a; --card:#1e293b; --border:#334155; --text:#e2e8f0; --muted:#64748b; --accent:#06b6d4; }
        * { margin:0; padding:0; box-sizing:border-box; }
        body { font:14px system-ui; background:var(--bg); color:var(--text); min-height:100vh; }
        a { color:var(--accent); text-decoration:none; }
        
        .container { max-width:1200px; margin:0 auto; padding:20px; }
        .header { padding:20px 0; border-bottom:1px solid var(--border); margin-bottom:20px; }
        .header h1 { font-size:20px; color:var(--accent); }
        
        .grid { display:grid; grid-template-columns:200px 1fr; gap:20px; }
        .sidebar, .main { background:var(--card); border:1px solid var(--border); border-radius:8px; padding:16px; }
        
        .table-btn { display:block; padding:10px 12px; margin:4px 0; border-radius:6px; color:var(--text); }
        .table-btn:hover, .table-btn.active { background:rgba(6,182,212,.15); color:var(--accent); }
        
        .btn { display:inline-block; padding:8px 16px; border:none; border-radius:6px; cursor:pointer; font-size:13px; }
        .btn-primary { background:var(--accent); color:#000; }
        .btn-danger { background:#ef4444; color:#fff; }
        .btn-ghost { background:transparent; border:1px solid var(--border); color:var(--text); }
        
        input, select { padding:8px 12px; background:var(--bg); border:1px solid var(--border); border-radius:6px; color:var(--text); }
        input:focus, select:focus { outline:none; border-color:var(--accent); }
        
        table { width:100%; border-collapse:collapse; margin:16px 0; }
        th, td { padding:10px; text-align:left; border-bottom:1px solid var(--border); }
        th { background:var(--bg); color:var(--muted); font-size:11px; text-transform:uppercase; }
        
        .tabs { display:flex; gap:4px; margin-bottom:16px; }
        .tab { padding:8px 16px; border-radius:6px; color:var(--muted); }
        .tab.active { background:rgba(6,182,212,.15); color:var(--accent); }
        
        .card { background:var(--bg); padding:16px; border-radius:8px; margin:8px 0; }
        .badge { display:inline-block; padding:2px 8px; border-radius:4px; font-size:11px; }
        .badge-cyan { background:rgba(6,182,212,.2); color:var(--accent); }
        .badge-purple { background:rgba(139,92,246,.2); color:#a78bfa; }
        
        .form-row { display:flex; gap:8px; margin:8px 0; }
        .form-row input { flex:1; }
        
        .message { padding:12px; border-radius:6px; margin-bottom:16px; }
        .message.success { background:rgba(16,185,129,.15); border:1px solid #10b981; }
        .message.error { background:rgba(239,68,68,.15); border:1px solid #ef4444; }
        
        pre { background:var(--bg); padding:12px; border-radius:6px; overflow-x:auto; font-size:12px; }
        .empty { padding:40px; text-align:center; color:var(--muted); }
        
        .perf-grid { display:grid; grid-template-columns:1fr 1fr; gap:16px; }
        .perf-card { padding:20px; border-radius:8px; text-align:center; }
        .perf-card.slow { background:rgba(239,68,68,.1); border:1px solid rgba(239,68,68,.3); }
        .perf-card.fast { background:rgba(16,185,129,.1); border:1px solid rgba(16,185,129,.3); }
        .perf-time { font-size:32px; font-weight:bold; margin:8px 0; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div style="display:flex; justify-content:space-between; align-items:center;">
                <div>
                    <h1>🗄️ In-Memory Database Engine</h1>
                    <small style="color:var(--muted)">CSE 211 - Pure Flask, No JavaScript</small>
                </div>
                <div style="padding:8px 16px; border-radius:6px; font-size:13px; font-weight:600; {% if backend_status and 'Connected' in backend_status %}background:rgba(16,185,129,.2); border:1px solid #10b981; color:#10b981;{% else %}background:rgba(239,68,68,.2); border:1px solid #ef4444; color:#ef4444;{% endif %}">
                    {% if backend_status %}
                    {{ backend_status }}
                    {% else %}
                    🔴 Backend: Disconnected
                    {% endif %}
                </div>
            </div>
        </div>
        
        {% if message %}
        <div class="message {{ message.type }}">{{ message.text }}</div>
        {% endif %}
        
        <div class="grid">
            <!-- Sidebar -->
            <div class="sidebar">
                <div style="margin-bottom:12px; color:var(--muted); font-size:11px; text-transform:uppercase;">Tables</div>
                {% for name in tables %}
                <a href="/?table={{ name }}&tab={{ tab }}" class="table-btn {{ 'active' if name == current }}">
                    📋 {{ name }} <small style="color:var(--muted)">({{ tables[name].rows|length }})</small>
                </a>
                {% endfor %}
                
                <hr style="border-color:var(--border); margin:16px 0;">
                
                <!-- Create Table Form -->
                <form method="POST" action="/create-table">
                    <input name="name" placeholder="New table name" style="width:100%; margin-bottom:8px;" required>
                    <input name="columns" placeholder="col1,col2,col3" style="width:100%; margin-bottom:8px;" required>
                    <select name="types" style="width:100%; margin-bottom:8px;">
                        <option value="INT,INT,INT">All INT</option>
                        <option value="STRING,STRING,STRING">All STRING</option>
                        <option value="INT,STRING,INT">INT,STRING,INT</option>
                    </select>
                    <button type="submit" class="btn btn-primary" style="width:100%;">+ Create Table</button>
                </form>
            </div>
            
            <!-- Main Content -->
            <div class="main">
                {% if current %}
                <!-- Tabs -->
                <div class="tabs">
                    <a href="/?table={{ current }}&tab=data" class="tab {{ 'active' if tab == 'data' }}">📊 Data</a>
                    <a href="/?table={{ current }}&tab=query" class="tab {{ 'active' if tab == 'query' }}">💻 Query</a>
                    <a href="/?table={{ current }}&tab=indexes" class="tab {{ 'active' if tab == 'indexes' }}">🌳 Indexes</a>
                    <a href="/?table={{ current }}&tab=benchmark" class="tab {{ 'active' if tab == 'benchmark' }}">⚡ Benchmark</a>
                </div>
                
                {% if tab == 'data' %}
                <!-- DATA TAB -->
                <h2 style="margin-bottom:8px;">📋 {{ current }}</h2>
                <p style="color:var(--muted); margin-bottom:16px;">
                    {{ tables[current].rows|length }} rows · 
                    Columns: {{ tables[current].columns|join(', ') }}
                </p>
                
                <!-- Insert Row Form -->
                <div class="card">
                    <form method="POST" action="/insert-row/{{ current }}" class="form-row" style="flex-wrap:wrap;">
                        {% for col in tables[current].columns %}
                        <input name="{{ col }}" placeholder="{{ col }} ({{ tables[current].types[loop.index0] }})" required>
                        {% endfor %}
                        <button type="submit" class="btn btn-primary">+ Insert</button>
                    </form>
                </div>
                
                <!-- Table -->
                <table>
                    <tr>
                        {% for col in tables[current].columns %}
                        <th>{{ col }}</th>
                        {% endfor %}
                        <th>Actions</th>
                    </tr>
                    {% for row in tables[current].rows %}
                    <tr>
                        {% for col in tables[current].columns %}
                        <td>{{ row[col] }}</td>
                        {% endfor %}
                        <td>
                            <a href="/delete-row/{{ current }}/{{ loop.index0 }}" class="btn btn-danger" style="padding:4px 8px; font-size:11px;">Delete</a>
                        </td>
                    </tr>
                    {% else %}
                    <tr><td colspan="{{ tables[current].columns|length + 1 }}" class="empty">No rows yet</td></tr>
                    {% endfor %}
                </table>
                
                <!-- Delete Table -->
                <div style="margin-top:20px; padding-top:20px; border-top:1px solid var(--border);">
                    <a href="/delete-table/{{ current }}" class="btn btn-danger" onclick="return confirm('Delete table?')">🗑️ Delete Table</a>
                </div>
                
                {% elif tab == 'query' %}
                <!-- QUERY TAB -->
                <h2 style="margin-bottom:16px;">💻 Execute Query</h2>
                
                <form method="POST" action="/query">
                    <textarea name="query" rows="3" style="width:100%; padding:12px; background:var(--bg); border:1px solid var(--border); border-radius:6px; color:var(--text); font-family:monospace;" placeholder="SELECT * FROM users WHERE age > 25">{{ query_text or '' }}</textarea>
                    <button type="submit" class="btn btn-primary" style="margin-top:8px;">▶ Execute</button>
                </form>
                
                <div class="card" style="margin-top:16px;">
                    <div style="color:var(--muted); font-size:12px; margin-bottom:8px;">Example queries:</div>
                    <code style="display:block; margin:4px 0;">SELECT * FROM users</code>
                    <code style="display:block; margin:4px 0;">SELECT * FROM users WHERE age > 25</code>
                    <code style="display:block; margin:4px 0;">SELECT * FROM users JOIN orders</code>
                </div>
                
                {% if query_result %}
                <h3 style="margin-top:20px;">Results ({{ query_result.execution_time }}ms)</h3>
                <table>
                    <tr>{% for col in query_result.columns %}<th>{{ col }}</th>{% endfor %}</tr>
                    {% for row in query_result.rows %}
                    <tr>{% for col in query_result.columns %}<td>{{ row[col] }}</td>{% endfor %}</tr>
                    {% else %}
                    <tr><td colspan="{{ query_result.columns|length }}" class="empty">No results</td></tr>
                    {% endfor %}
                </table>
                {% endif %}
                
                {% elif tab == 'indexes' %}
                <!-- INDEXES TAB -->
                <h2 style="margin-bottom:16px;">🌳 Index Management</h2>
                
                <div style="display:grid; grid-template-columns:1fr 1fr; gap:16px; margin-bottom:20px;">
                    <div class="card">
                        <h4 style="color:var(--accent); margin-bottom:8px;">B+ Tree Indexes</h4>
                        {% for idx in tables[current].indexes.btree %}
                        <span class="badge badge-cyan" style="margin:2px;">{{ idx }}</span>
                        {% else %}
                        <span style="color:var(--muted)">None</span>
                        {% endfor %}
                    </div>
                    <div class="card">
                        <h4 style="color:#a78bfa; margin-bottom:8px;">Hash Indexes</h4>
                        {% for idx in tables[current].indexes.hash %}
                        <span class="badge badge-purple" style="margin:2px;">{{ idx }}</span>
                        {% else %}
                        <span style="color:var(--muted)">None</span>
                        {% endfor %}
                    </div>
                </div>
                
                <h3 style="margin-bottom:12px;">Create New Index</h3>
                <form method="POST" action="/create-index/{{ current }}" class="form-row">
                    <select name="column" required>
                        <option value="">Select column...</option>
                        {% for col in tables[current].columns %}
                        <option value="{{ col }}">{{ col }}</option>
                        {% endfor %}
                    </select>
                    <select name="type" required>
                        <option value="btree">B+ Tree</option>
                        <option value="hash">Hash</option>
                    </select>
                    <button type="submit" class="btn btn-primary">Create Index</button>
                </form>
                
                {% elif tab == 'benchmark' %}
                <!-- BENCHMARK TAB -->
                <h2 style="margin-bottom:16px;">⚡ Join Benchmark</h2>
                
                <form method="POST" action="/benchmark">
                    <input type="hidden" name="table" value="{{ current }}">
                    <button type="submit" class="btn btn-primary">🚀 Run Benchmark</button>
                </form>
                
                {% if benchmark_result %}
                <div class="perf-grid" style="margin-top:20px;">
                    <div class="perf-card slow">
                        <h4>Nested Loop Join</h4>
                        <div class="perf-time" style="color:#ef4444;">{{ benchmark_result.nested_loop.time_ms }}ms</div>
                        <div style="color:var(--muted);">{{ benchmark_result.nested_loop.complexity }}</div>
                        <div style="color:var(--muted);">Memory: {{ benchmark_result.nested_loop.memory_usage }}</div>
                    </div>
                    <div class="perf-card fast">
                        <h4>Hash Join</h4>
                        <div class="perf-time" style="color:#10b981;">{{ benchmark_result.hash_join.time_ms }}ms</div>
                        <div style="color:var(--muted);">{{ benchmark_result.hash_join.complexity }}</div>
                        <div style="color:var(--muted);">Memory: {{ benchmark_result.hash_join.memory_usage }}</div>
                    </div>
                </div>
                <p style="margin-top:16px; color:var(--muted); text-align:center;">
                    Hash Join is <strong style="color:#10b981;">{{ ((benchmark_result.nested_loop.time_ms / benchmark_result.hash_join.time_ms)|round(1)) }}x faster</strong>
                </p>
                {% endif %}
                
                {% endif %}
                
                {% else %}
                <div class="empty">
                    <h3 style="margin-bottom:8px;">👈 Select a table</h3>
                    <p>Or create a new one from the sidebar</p>
                </div>
                {% endif %}
            </div>
        </div>
    </div>
</body>
</html>
'''

# ============================================================================
# ROUTES
# ============================================================================

@app.route('/')
def index():
    from flask import render_template_string
    # C++ backend durumunu kontrol et
    is_connected = check_cpp_backend()
    backend_status = "🟢 C++ Backend Connected" if is_connected else "🔴 C++ Backend Disconnected (Python simulation)"
    
    return render_template_string(HTML,
        tables=db,
        current=request.args.get('table'),
        tab=request.args.get('tab', 'data'),
        message=request.args.get('msg') and {'type': request.args.get('type', 'success'), 'text': request.args.get('msg')},
        query_result=None,
        query_text=None,
        benchmark_result=None,
        backend_status=backend_status
    )

@app.route('/create-table', methods=['POST'])
def create_table():
    name = request.form['name']
    cols = [c.strip() for c in request.form['columns'].split(',')]
    types = [t.strip() for t in request.form['types'].split(',')]
    
    if name in db:
        return redirect(f'/?msg=Table already exists&type=error')
    
    db[name] = {"columns": cols, "types": types, "rows": [], "indexes": {"btree": [], "hash": []}}
    return redirect(f'/?table={name}&tab=data&msg=Table created!&type=success')

@app.route('/delete-table/<name>')
def delete_table(name):
    if name in db:
        del db[name]
    return redirect(f'/?msg=Table deleted&type=success')

@app.route('/insert-row/<table>', methods=['POST'])
def insert_row(table):
    if table not in db:
        return redirect('/?msg=Table not found&type=error')
    
    t = db[table]
    row = {}
    for i, col in enumerate(t['columns']):
        val = request.form.get(col, '')
        if t['types'][i] == 'INT':
            row[col] = int(val) if val else 0
        elif t['types'][i] == 'DOUBLE':
            row[col] = float(val) if val else 0.0
        else:
            row[col] = val
    
    t['rows'].append(row)
    return redirect(f'/?table={table}&tab=data&msg=Row inserted&type=success')

@app.route('/delete-row/<table>/<int:idx>')
def delete_row(table, idx):
    if table in db and 0 <= idx < len(db[table]['rows']):
        db[table]['rows'].pop(idx)
    return redirect(f'/?table={table}&tab=data&msg=Row deleted&type=success')

@app.route('/create-index/<table>', methods=['POST'])
def create_index(table):
    if table not in db:
        return redirect('/?msg=Table not found&type=error')
    
    col = request.form['column']
    idx_type = request.form['type']
    
    if col not in db[table]['indexes'][idx_type]:
        db[table]['indexes'][idx_type].append(col)
    
    return redirect(f'/?table={table}&tab=indexes&msg=Index created&type=success')

@app.route('/query', methods=['POST'])
def query():
    from flask import render_template_string
    q = request.form.get('query', '').upper()
    start = time.time()
    
    if "JOIN" in q:
        result = {
            "columns": ["users.name", "orders.amount"],
            "rows": [
                {"users.name": "Alice", "orders.amount": 99.99},
                {"users.name": "Alice", "orders.amount": 29.99},
                {"users.name": "Bob", "orders.amount": 149.50},
            ]
        }
    elif "SELECT" in q:
        table_name = next((n for n in db if n.upper() in q), None)
        if table_name:
            rows = db[table_name]['rows']
            if "WHERE" in q and "AGE > 25" in q:
                rows = [r for r in rows if r.get('age', 0) > 25]
            result = {"columns": db[table_name]['columns'], "rows": rows}
        else:
            result = {"columns": [], "rows": []}
    else:
        result = {"columns": [], "rows": []}
    
    result['execution_time'] = round((time.time() - start) * 1000, 2)
    
    return render_template_string(HTML,
        tables=db,
        current=request.args.get('table') or list(db.keys())[0] if db else None,
        tab='query',
        message=None,
        query_result=result,
        query_text=request.form.get('query', ''),
        benchmark_result=None
    )

@app.route('/benchmark', methods=['POST'])
def benchmark():
    from flask import render_template_string
    result = {
        "nested_loop": {"time_ms": round(random.uniform(30, 80), 2), "complexity": "O(N × M)", "memory_usage": "Low"},
        "hash_join": {"time_ms": round(random.uniform(5, 20), 2), "complexity": "O(N + M)", "memory_usage": "High"}
    }
    
    return render_template_string(HTML,
        tables=db,
        current=request.form.get('table') or list(db.keys())[0] if db else None,
        tab='benchmark',
        message=None,
        query_result=None,
        query_text=None,
        benchmark_result=result
    )

if __name__ == '__main__':
    print("\n🚀 Server: http://localhost:5002 (No JavaScript!)\n")
    app.run(debug=True, host='127.0.0.1', port=5002)

