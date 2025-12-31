# 🗄️ In-Memory Database Engine - Python API GUI

CSE 211 - Data Structures Project için Python Flask API ve Web GUI.

---

## 🚀 Nasıl Çalıştırılır (3 Adım!)

### Adım 1: Dosyaları indir
- `app.py`
- `requirements.txt`

Her iki dosyayı aynı klasöre koy.

### Adım 2: Kütüphaneleri yükle
Terminal/CMD aç ve şu komutu çalıştır:

```bash
pip install -r requirements.txt
```

veya:

```bash
pip install flask flask-cors
```

### Adım 3: Sunucuyu başlat
```bash
python app.py
```

### Adım 4: Tarayıcıda aç
```
http://localhost:5000
```

---

## 📸 Ne Göreceksin

```
┌─────────────────────────────────────────────────────┐
│  🗄️ In-Memory Database Engine     ● API Connected   │
├──────────┬──────────────────────────────────────────┤
│ TABLES   │  📊 Table View │ 💻 Query │ 🌳 Indexes   │
│          │──────────────────────────────────────────│
│ 📋 users │                                          │
│ 📋 orders│  Tablo görüntüleme, satır ekleme/silme   │
│          │  SQL sorguları çalıştırma                │
│ [+ New]  │  Index yönetimi (B+ Tree, Hash)          │
│          │  Join benchmark karşılaştırması          │
└──────────┴──────────────────────────────────────────┘
```

---

## 📡 API Endpoints

| Method | Endpoint | Açıklama |
|--------|----------|----------|
| GET | `/api/tables` | Tüm tabloları listele |
| GET | `/api/tables/<name>` | Belirli tabloyu getir |
| POST | `/api/tables` | Yeni tablo oluştur |
| DELETE | `/api/tables/<name>` | Tabloyu sil |
| GET | `/api/tables/<name>/rows` | Satırları getir |
| POST | `/api/tables/<name>/rows` | Yeni satır ekle |
| DELETE | `/api/tables/<name>/rows/<index>` | Satır sil |
| POST | `/api/tables/<name>/indexes` | Index oluştur |
| POST | `/api/query` | SQL sorgusu çalıştır |
| POST | `/api/benchmark/join` | Join karşılaştırması |

---

## 🔗 C++ Backend'e Bağlama (Gelecek Adım)

Bu API şu anda Python'da simüle ediyor. C++ kodunuza bağlamak için:

### Seçenek 1: pybind11 (Önerilen)

```cpp
// C++ tarafı
#include <pybind11/pybind11.h>

PYBIND11_MODULE(database_cpp, m) {
    py::class_<Table>(m, "Table")
        .def("insert_row", &Table::insertRow)
        .def("get_rows", &Table::getRows);
}
```

```python
# Python tarafı (app.py içinde)
import database_cpp

@app.route('/api/tables/<name>/rows', methods=['POST'])
def insert_row(name):
    # C++ fonksiyonunu çağır
    cpp_table = database_cpp.Table(name)
    cpp_table.insert_row(request.json)
    return jsonify({"success": True})
```

### Seçenek 2: ctypes

```python
import ctypes

# C++ shared library'yi yükle
lib = ctypes.CDLL('./libdatabase.so')

# Fonksiyonları çağır
lib.insert_row(table_name, row_data)
```

---

## 📁 Proje Yapısı

```
project/
├── app.py              ← Flask API + HTML GUI
├── requirements.txt    ← Python bağımlılıkları
├── README.md           ← Bu dosya
│
├── include/            ← C++ header dosyaları
│   ├── core/
│   │   ├── Table.hpp
│   │   ├── Row.hpp
│   │   └── Cell.hpp
│   ├── index/
│   │   ├── BPlusTree.hpp
│   │   └── HashIndex.hpp
│   └── engine/
│       └── query/
│           ├── query_parser.hpp
│           └── join_engine.hpp
│
├── src/                ← C++ source dosyaları
│   └── ...
│
└── data/               ← Test verileri
    └── final_test_db.json
```

---

## ✨ Özellikler

- ✅ Tablo oluşturma/silme
- ✅ Satır ekleme/silme
- ✅ SQL sorgu çalıştırma (SELECT, WHERE, JOIN)
- ✅ B+ Tree ve Hash Index yönetimi
- ✅ Nested Loop vs Hash Join karşılaştırması
- ✅ Modern, karanlık tema UI
- ✅ Gerçek zamanlı API iletişimi

---

## 🛠️ Gereksinimler

- Python 3.8+
- Flask
- flask-cors
- Modern web tarayıcı

---

## 📝 Notlar

1. **Port değiştirmek için:** `app.py` dosyasının en altında `port=5000` değerini değiştirin.

2. **Debug modu kapatmak için:** `debug=True` → `debug=False`

3. **Hata alıyorsanız:**
   - Python'un yüklü olduğundan emin olun: `python --version`
   - Flask'ın yüklü olduğundan emin olun: `pip show flask`
   - Port 5000'in kullanılmadığından emin olun

---

## 👨‍💻 Yazar

CSE 211 - Data Structures Term Project
Yeditepe University - 2025-2026 Fall
