/**
 * @file BPlusTree.hpp
 * @brief PROJ-19 için Custom B+ Tree Header Dosyası.
 * STL Vector KULLANILMAZ. Tamamen dinamik diziler (new[]) ve pointerlar üzerine kuruludur.
 */

#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include "../core/Cell.hpp"
#include "../core/Row.hpp"

// ==========================================
// BPNode (B+ Tree Düğümü)
// ==========================================
struct BPNode {
    bool isLeaf;        // Yaprak mı?
    int numKeys;        // Mevcut anahtar sayısı
    
    Cell* keys;         // Anahtarları tutan dinamik dizi (Array of Cells)
    BPNode* next;       // Sadece yapraklar için: Bir sonraki yaprağa pointer (Linked List)

    // Union kullanmak bellek tasarrufu sağlar ama 
    // implementasyon kolaylığı için ayrı pointerlar olarak tanımlıyoruz:
    Row** values;       // Sadece yapraklarda: Satır verilerini tutan array
    BPNode** children;  // Sadece iç düğümlerde: Çocukları tutan array

    // Constructor & Destructor
    BPNode(int maxDegree, bool leaf);
    ~BPNode();
};

// ==========================================
// BPlusTree Sınıfı
// ==========================================
class BPlusTree {
private:
    BPNode* root;
    int degree; // Ağacın derecesi (maksimum çocuk sayısı veya düzen)

    // --- Yardımcı Fonksiyonlar (Private Helpers) ---
    
    // Dolu olmayan bir düğüme ekleme yapar
    void insertNonFull(BPNode* node, const Cell& key, Row* value);

    // Dolu bir yaprak düğümü böler
    void splitLeaf(BPNode* parent, int index, BPNode* leaf);

    // Dolu bir iç düğümü (internal node) böler
    void splitInternal(BPNode* parent, int index, BPNode* internalNode);

public:
    // Constructor (Varsayılan derece 3)
    BPlusTree(int deg = 3);
    
    // Destructor
    ~BPlusTree();

    // --- Temel Operasyonlar ---
    
    // Anahtara göre satır bulur (Equality Search)
    Row* search(const Cell& key);

    // Yeni veri ekler
    void insert(const Cell& key, Row* value);

    // Ağacı (yaprakları) sıralı yazdırır
    void print();
};

#endif // BPLUSTREE_HPP