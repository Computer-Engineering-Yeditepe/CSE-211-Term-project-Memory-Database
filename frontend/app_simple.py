from flask import Flask, request, redirect, url_for, render_template_string
import requests
import json

app = Flask(__name__)

CPP_BACKEND_URL = "http://localhost:8080"

HTML = '''
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>DB Engine Dashboard</title>
    <style>
        :root { --bg:#0f172a; --card:#1e293b; --border:#334155; --text:#e2e8f0; --muted:#64748b; --accent:#06b6d4; }
        * { margin:0; padding:0; box-sizing:border-box; }
        body { font:14px system-ui; background:var(--bg); color:var(--text); min-height:100vh; }
        a { color:var(--accent); text-decoration:none; }
        .container { max-width:1200px; margin:0 auto; padding:20px; }
        .header { padding:20px 0; border-bottom:1px solid var(--border); margin-bottom:20px; }
        .grid { display:grid; grid-template-columns:200px 1fr; gap:20px; }
        .sidebar, .main { background:var(--card); border:1px solid var(--border); border-radius:8px; padding:16px; }
        .table-btn { display:block; padding:10px 12px; margin:4px 0; border-radius:6px; color:var(--text); }
        .table-btn.active { background:rgba(6,182,212,.15); color:var(--accent); }
        .btn { display:inline-block; padding:8px 16px; border:none; border-radius:6px; cursor:pointer; background:var(--accent); color:#000; }
        .btn-danger { background:#ef4444; color:#fff; }
        input, select, textarea { padding:8px; background:var(--bg); border:1px solid var(--border); color:var(--text); width:100%; border-radius:4px; margin-bottom:5px;}
        table { width:100%; border-collapse:collapse; margin-top:10px; }
        th, td { padding:10px; text-align:left; border-bottom:1px solid var(--border); }
        th { background:var(--bg); color:var(--muted); }
        .tabs { display:flex; gap:10px; margin-bottom:15px; }
        .tab { padding:5px 10px; color:var(--muted); }
        .tab.active { border-bottom:2px solid var(--accent); color:var(--accent); }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🗄️ C++ Database Manager</h1>
            <span style="color: {{ 'limegreen' if connected else 'red' }}">
                {{ '🟢 Connected' if connected else '🔴 Disconnected' }}
            </span>
        </div>

        <div class="grid">
            <div class="sidebar">
                <h3>Tables</h3>
                {% for name in tables %}
                <a href="/?table={{ name }}" class="table-btn {{ 'active' if name == current }}">{{ name }}</a>
                {% endfor %}
                <hr style="border-color:var(--border); margin:15px 0;">
                <form action="/create-table" method="POST">
                    <input name="name" placeholder="Table Name" required>
                    <input name="columns" placeholder="col1,col2" required>
                    <input name="types" placeholder="INT,STRING" required>
                    <button class="btn">+ Create (Resets DB)</button>
                </form>
            </div>
            
            <div class="main">
                {% if current %}
                <div class="tabs">
                    <a href="/?table={{ current }}&tab=data" class="tab {{ 'active' if tab == 'data' }}">Data</a>
                    <a href="/?table={{ current }}&tab=query" class="tab {{ 'active' if tab == 'query' }}">SQL Query</a>
                </div>

                {% if tab == 'data' %}
                    <form action="/insert-row/{{ current }}" method="POST" style="display:flex; gap:5px; margin-bottom:20px;">
                        {% for col in tables[current].columns %}
                        <input name="{{ col }}" placeholder="{{ col }} ({{ tables[current].types[loop.index0] }})">
                        {% endfor %}
                        <button class="btn">Insert</button>
                    </form>

                    <table>
                        <tr>
                            {% for col in tables[current].columns %}<th>{{ col }}</th>{% endfor %}
                            <th>Action</th>
                        </tr>
                        {% for row in tables[current].rows %}
                        <tr>
                            {% for col in tables[current].columns %}
                                <td>
                                {% if row[col] is mapping %}
                                    {{ row[col] }} 
                                {% else %}
                                    {{ row[col] }}
                                {% endif %}
                                </td>
                            {% endfor %}
                            <td>
                                <a href="/delete-row/{{ row.id if row.id else 0 }}" class="btn btn-danger" style="padding:4px 8px;">Del</a>
                            </td>
                        </tr>
                        {% endfor %}
                    </table>

                {% elif tab == 'query' %}
                    <h3>Execute SQL</h3>
                    <form method="POST" action="/query">
                        <input type="hidden" name="table" value="{{ current }}">
                        <textarea name="query" rows="4">{{ last_query }}</textarea>
                        <button class="btn" style="margin-top:10px;">Run Query</button>
                    </form>
                    
                    {% if query_result %}
                    <div style="margin-top:20px; padding:10px; background:rgba(0,0,0,0.2); border-radius:5px;">
                        <strong>Result from C++:</strong>
                        <pre>{{ query_result }}</pre>
                    </div>
                    {% endif %}
                {% endif %}

                {% else %}
                <p>No table selected or database empty.</p>
                {% endif %}
            </div>
        </div>
    </div>
</body>
</html>
'''

local_db = {}

@app.route('/')
def index():
    global local_db
    connected = False
    try:
        r = requests.get(f"{CPP_BACKEND_URL}/get_all", timeout=0.5)
        if r.status_code == 200:
            local_db = r.json()
            connected = True
    except:
        pass

    current_table = request.args.get('table')
    if not current_table and local_db:
        current_table = list(local_db.keys())[0]

    return render_template_string(HTML,
        tables=local_db,
        current=current_table,
        tab=request.args.get('tab', 'data'),
        connected=connected,
        last_query=request.args.get('last_query', ''),
        query_result=request.args.get('query_result', '')
    )

@app.route('/create-table', methods=['POST'])
def create_table():
    try:
        # data yerine params kullanıyoruz (GET)
        requests.get(f"{CPP_BACKEND_URL}/create_table", params=request.form)
    except:
        pass
    return redirect('/')

@app.route('/insert-row/<table>', methods=['POST'])
def insert_row(table):
    try:
        # data yerine params kullanıyoruz (GET)
        requests.get(f"{CPP_BACKEND_URL}/insert", params=request.form)
    except:
        pass
    return redirect(f'/?table={table}')

@app.route('/delete-row/<int:id>')
def delete_row(id):
    try:
        # data yerine params kullanıyoruz (GET)
        requests.get(f"{CPP_BACKEND_URL}/delete", params={'id': id})
    except:
        pass
    return redirect('/')

@app.route('/query', methods=['POST'])
def query():
    query_text = request.form.get('query')
    current_table = request.form.get('table')
    result_msg = ""
    
    try:
        # data yerine params kullanıyoruz (GET)
        r = requests.get(f"{CPP_BACKEND_URL}/query", params={'query': query_text})
        if r.status_code == 200:
            result_msg = r.json().get('msg', 'Success')
    except Exception as e:
        result_msg = f"Error: {e}"

    return redirect(url_for('index', table=current_table, tab='query', last_query=query_text, query_result=result_msg))

if __name__ == '__main__':
    app.run(debug=True, port=5002)