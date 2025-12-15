#include "../../include/indexes/BPlusTree.hpp"
#include <iostream>

// ==========================================
// BPNode (Düğüm) Implementasyonu
// ==========================================

BPNode::BPNode(int maxDegree, bool leaf) {
    isLeaf = leaf;
    numKeys = 0;
    
    // Pointer dizilerini oluştur
    keys = new Cell[maxDegree]; 
    next = nullptr;

    // Bellek güvenliği için dizileri NULL ile başlat
    if (leaf) {
        values = new Row*[maxDegree];
        children = nullptr;
    } else {
        children = new BPNode*[maxDegree + 1];
        values = nullptr;
    }
}

BPNode::~BPNode() {
    delete[] keys;
    if (isLeaf) delete[] values;
    else delete[] children;
}

// ==========================================
// BPlusTree (Ağaç) Implementasyonu
// ==========================================

BPlusTree::BPlusTree(int deg) : root(nullptr), degree(deg) {}

BPlusTree::~BPlusTree() {
    // OS belleği temizler, karmaşıklık olmasın diye boş bırakıldı
}

// --- ARAMA (SEARCH) ---
Row* BPlusTree::search(const Cell& key) {
    if (root == nullptr) return nullptr;

    BPNode* curr = root;

    // 1. Yaprağa kadar in
    while (!curr->isLeaf) {
        int i = 0;
        // Dizi taşmasını önlemek için i < numKeys kontrolü şart
        while (i < curr->numKeys && key > curr->keys[i]) {
            i++;
        }
        curr = curr->children[i];
    }

    // 2. Yaprakta (Leaf) tam eşleşme ara
    for (int i = 0; i < curr->numKeys; i++) {
        if (curr->keys[i] == key) {
            return curr->values[i];
        }
    }
    return nullptr;
}

// --- EKLEME (INSERT) - CLRS YÖNTEMİ ---
void BPlusTree::insert(const Cell& key, Row* value) {
    // 1. Ağaç boşsa
    if (root == nullptr) {
        root = new BPNode(degree, true);
        root->keys[0] = key;
        root->values[0] = value;
        root->numKeys = 1;
        return;
    }

    // 2. Kök doluysa (Bölünmeli)
    if (root->numKeys == degree - 1) {
        BPNode* newRoot = new BPNode(degree, false); // Yeni boş kök (İç düğüm)
        newRoot->children[0] = root;                 // Eski kök bunun çocuğu olur
        
        // Eski kökü böl (Split Child)
        splitInternal(newRoot, 0, root); // root artık newRoot'un çocuğu oldu
        
        // Yeni kök üzerinden eklemeye devam et
        root = newRoot;
        insertNonFull(root, key, value);
    } else {
        // Kök dolu değilse direkt ekle
        insertNonFull(root, key, value);
    }
}

// Dolu olmayan düğüme ekle
void BPlusTree::insertNonFull(BPNode* node, const Cell& key, Row* value) {
    int i = node->numKeys - 1;

    if (node->isLeaf) {
        // --- YAPRAK DÜĞÜM ---
        // Verileri kaydırarak yer aç
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        // Yerleş
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
        node->numKeys++;
    } 
    else {
        // --- İÇ DÜĞÜM ---
        // Hangi çocuğa ineceğimizi bul
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++; // Doğru çocuk indisi

        // İneceğimiz çocuk dolu mu?
        if (node->children[i]->numKeys == degree - 1) {
            // Doluysa inmeden önce böl (Preemptive Split)
            if (node->children[i]->isLeaf) {
                splitLeaf(node, i, node->children[i]);
            } else {
                splitInternal(node, i, node->children[i]);
            }

            // Bölünme sonrası ortadaki anahtar yukarı çıktı.
            // Ekleyeceğimiz değer bu yukarı çıkan anahtardan büyükse sağa gitmeliyiz.
            if (node->keys[i] < key) {
                i++;
            }
        }
        // Artık çocuğun yeri garanti, oraya in
        insertNonFull(node->children[i], key, value);
    }
}

// YAPRAK BÖLME
void BPlusTree::splitLeaf(BPNode* parent, int index, BPNode* leaf) {
    BPNode* newLeaf = new BPNode(degree, true);
    
    // Derecenin yarısı (t-1 gibi düşünelim)
    int mid = degree / 2; 
    
    // Sağ taraftaki eleman sayısı
    newLeaf->numKeys = degree - 1 - mid; // Düzeltildi

    // 1. Verileri sağa taşı
    // (mid'den başlayarak sonuna kadar)
    for (int j = 0; j < newLeaf->numKeys; j++) {
        newLeaf->keys[j] = leaf->keys[mid + j];
        newLeaf->values[j] = leaf->values[mid + j];
    }

    // Sol tarafın boyutunu güncelle
    leaf->numKeys = mid;

    // 2. Bağlı Liste (Linked List) Zincirini Güncelle
    newLeaf->next = leaf->next;
    leaf->next = newLeaf;

    // 3. Parent (Ebeveyn) Güncelleme
    // Parent içindeki çocukları sağa kaydır
    for (int j = parent->numKeys; j >= index + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    // Yeni çocuğu bağla
    parent->children[index + 1] = newLeaf;

    // Parent içindeki anahtarları sağa kaydır
    for (int j = parent->numKeys - 1; j >= index; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    
    // B+ Tree Kuralı: Yaprak bölünürken ortadaki eleman YUKARI KOPYALANIR
    parent->keys[index] = newLeaf->keys[0];
    parent->numKeys++;
}

// İÇ DÜĞÜM BÖLME
void BPlusTree::splitInternal(BPNode* parent, int index, BPNode* internalNode) {
    BPNode* newInternal = new BPNode(degree, false);
    int mid = degree / 2;

    // Sağ tarafa geçecek eleman sayısı
    // Not: mid indeksindeki eleman yukarı gidecek, o yüzden -1 yapıyoruz
    newInternal->numKeys = degree - 1 - mid - 1; 

    // 1. Anahtarları sağa taşı (mid+1'den başla)
    for (int j = 0; j < newInternal->numKeys; j++) {
        newInternal->keys[j] = internalNode->keys[mid + 1 + j];
    }

    // 2. Çocukları sağa taşı (mid+1'den başla)
    for (int j = 0; j < newInternal->numKeys + 1; j++) {
        newInternal->children[j] = internalNode->children[mid + 1 + j];
    }

    // Soldakilerin sayısı
    internalNode->numKeys = mid;

    // 3. Parent Güncelleme (Çocukları kaydır)
    for (int j = parent->numKeys; j >= index + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = newInternal;

    // Anahtarları kaydır
    for (int j = parent->numKeys - 1; j >= index; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }

    // B+ Tree Kuralı: İç düğüm bölünürken ortadaki eleman YUKARI TAŞINIR (Kopyalanmaz)
    parent->keys[index] = internalNode->keys[mid];
    parent->numKeys++;
}

// --- YAZDIRMA (BAĞLI LİSTE MANTIĞI) ---
void BPlusTree::print() {
    if (root == nullptr) return;

    // 1. En sol yaprağı bulana kadar in
    BPNode* curr = root;
    while (!curr->isLeaf) {
        curr = curr->children[0];
    }

    // 2. Yaprakları next pointer ile gez (Linked List Gezintisi)
    while (curr != nullptr) {
        for (int i = 0; i < curr->numKeys; i++) {
            std::cout << curr->keys[i] << " ";
        }
        curr = curr->next;
    }
    std::cout << std::endl;
}