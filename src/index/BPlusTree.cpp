#include "../../include/index/BPlusTree.hpp"



namespace index {
    BPlusTree::BPlusTree(int _degree) {
        this->degree = _degree;
        this->root = new BPlusNode(degree, true); 
    }

    BPlusTree::~BPlusTree() {
        if (root != nullptr) {
        removeTree(root); 
        }
    }

    void BPlusTree::removeTree(BPlusNode* node) {
        if (node == nullptr) return;

        if(!node->is_leaf){
            for (int i = 0; i <= node->key_count; i++) {
                removeTree(node->children[i]);
            }
        }

        delete node;

    }

    RecordID BPlusTree::search(int key) {
        BPlusNode* current = root;

        while (!current->is_leaf) {
            int i = 0;
            while (i < current->key_count && key >= current->keys[i]) {
                i++;
            }
            current = current->children[i];
        }

        for (int i = 0; i < current->key_count; i++) {
            if (current->keys[i] == key) {
                return current->values[i];
            }
        }

        return RecordID{-1, -1}; 
    }

    void BPlusTree::insert(int key, RecordID value) {
            if (root == nullptr) {

            root = new BPlusNode(degree, true); 
            root->keys[0] = key;
            root->values[0] = value;
            root->key_count = 1;
            return;
        
        }
        
        if (root->key_count == degree - 1) {
            
            BPlusNode* newRoot = new BPlusNode(degree, false);
            
            newRoot->children[0] = root;
            
            splitChild(newRoot, 0, root);
            
            int i = 0;
            if (newRoot->keys[0] < key) {
                i++;
            }
            
            insertNonFull(newRoot->children[i], key, value);

            root = newRoot;
        } 
        else {
            insertNonFull(root, key, value);
        }


        }

    void BPlusTree::insertNonFull(BPlusNode* node, int key, RecordID value) {
        int i = node->key_count - 1; 

        if (node->is_leaf) {
            
            while (i >= 0 && node->keys[i] > key) {
                node->keys[i + 1] = node->keys[i];     
                node->values[i + 1] = node->values[i]; 
                i--;
            }

            
            node->keys[i + 1] = key;
            node->values[i + 1] = value;
            node->key_count++;
        } 
        else {
            
            while (i >= 0 && node->keys[i] > key) {
                i--;
            }
            i++; 

            
            if (node->children[i]->key_count == degree - 1) {
                
                splitChild(node, i, node->children[i]);

                
                if (node->keys[i] < key) {
                    i++;
                }
            }
            
            insertNonFull(node->children[i], key, value);
        }
    }

    void BPlusTree::splitChild(BPlusNode* parent, int index, BPlusNode* child) {
        
        BPlusNode* z = new BPlusNode(child->max_degree, child->is_leaf);
        
        
        int t = degree / 2; 

        
        if (child->is_leaf) {
            
            z->key_count = child->key_count - t;
            
            for (int j = 0; j < z->key_count; j++) {
                z->keys[j] = child->keys[j + t];     
                z->values[j] = child->values[j + t]; 
            }
            
            
            z->next = child->next;
            child->next = z;
        } 
        else {
            
            z->key_count = child->key_count - t - 1; 

            for (int j = 0; j < z->key_count; j++) {
                z->keys[j] = child->keys[j + t + 1];
            }
            
            
            for (int j = 0; j <= z->key_count; j++) {
                z->children[j] = child->children[j + t + 1];
            }
        }

        
        for (int j = parent->key_count; j >= index + 1; j--) {
            parent->children[j + 1] = parent->children[j];
        }
        
        parent->children[index + 1] = z;

        
        for (int j = parent->key_count - 1; j >= index; j--) {
            parent->keys[j + 1] = parent->keys[j];
        }

        
        parent->keys[index] = child->keys[t];
        
        
        parent->key_count++;
        
        
        if (child->is_leaf) {
            child->key_count = t; 
        } else {
            child->key_count = t; 
            
        }
    }

    void BPlusTree::searchRange(int minKey, int maxKey) {
        if (root == nullptr) return;

        BPlusNode* current = root;

        while (!current->is_leaf) {
            int i = 0;
            while (i < current->key_count && minKey >= current->keys[i]) {
                i++;
            }
            current = current->children[i];
        }

       
        bool finished = false;
        std::cout << "Range Search [" << minKey << " - " << maxKey << "]: \n";
        
        while (current != nullptr && !finished) {
            for (int i = 0; i < current->key_count; i++) {
                
                if (current->keys[i] >= minKey && current->keys[i] <= maxKey) {
                    std::cout << " -> Key: " << current->keys[i] 
                              << " (Page: " << current->values[i].page_id 
                              << ", Slot: " << current->values[i].slot_id << ")\n";
                }
                
                if (current->keys[i] > maxKey) {
                    finished = true;
                    break;
                }
            }
            
            current = current->next; 
        }
        std::cout << "-----------------------------------\n";
    }

    void BPlusTree::remove(int key) {
        if (root == nullptr) {
            std::cout << "Agac bos, silinecek veri yok.\n";
            return;
        }

        removeInternal(root, key);

        if (root->key_count == 0) {
            BPlusNode* tmp = root;
            if (root->is_leaf) {
                root = nullptr;
            } else {

                root = root->children[0];
            }
            delete tmp; 
        }
    }

    void BPlusTree::removeInternal(BPlusNode* node, int key) {
        int idx = 0;
        // İneceğimiz dalı veya silinecek anahtarı bul
        while (idx < node->key_count && node->keys[idx] < key) {
            idx++;
        }

        // DURUM 1: Yaprak Düğümdeyiz (Asıl Silme Yeri)
        if (node->is_leaf) {
            if (idx < node->key_count && node->keys[idx] == key) {
                removeFromLeaf(node, idx);
            } else {
                std::cout << "Hata: Anahtar " << key << " bulunamadi.\n";
            }
            return;
        }

        // DURUM 2: İç Düğümdeyiz (Yönlendirme)
        // B+ Tree Farkı: Eğer keys[idx] == key ise, aradığımız veri bu anahtarın
        // sağ tarafındaki alt ağaçtadır (children[idx+1]).
        // Bu yüzden iç düğümde bulsak bile silmiyoruz, sağa yöneliyoruz.
        if (idx < node->key_count && node->keys[idx] == key) {
            idx++; 
        }

        // Doluluk Kontrolü (Underflow Protection)
        // İneceğimiz çocuk "Fakir" ise (yarıdan az eleman), onu doldur (fill).
        if (node->children[idx]->key_count < degree / 2) {
            fill(node, idx);
            
            // Fill işlemi (Merge/Borrow) ağaç yapısını değiştirebilir.
            // İneceğimiz çocuğun indeksi kaymış olabilir.
            // En güvenlisi: key'e göre tekrar yönü bulmaktır.
            if (idx > node->key_count) idx = node->key_count; // Basit sınır kontrolü
            // Daha hassas kontrol için while döngüsü tekrar çalıştırılabilir ama
            // çoğu durumda fill işlemi sonrası idx ya aynı kalır ya bir azalır.
        }

        // Fill işleminden sonra, ağaç yapısı değişmiş olabileceği için 
        // anahtarın hangi çocukta kaldığını tekrar doğrulayalım.
        int direction = 0;
        while (direction < node->key_count && node->keys[direction] < key) {
            direction++;
        }
        // Eğer tam ayırıcıdaysak sağa git (B+ Tree kuralı)
        if (direction < node->key_count && node->keys[direction] == key) {
            direction++;
        }

        // Rekürsif olarak aşağı in
        removeInternal(node->children[direction], key);
    }

    void BPlusTree::removeFromLeaf(BPlusNode* node, int idx) {
        for (int i = idx + 1; i < node->key_count; ++i) {
            node->keys[i - 1] = node->keys[i];
            node->values[i - 1] = node->values[i];
        }
        node->key_count--;
    }

    void BPlusTree::removeFromNonLeaf(BPlusNode* node, int idx) {
        int k = node->keys[idx];

        
        if (node->children[idx]->key_count >= degree / 2) {
            int pred = getPred(node, idx);
            node->keys[idx] = pred;
            removeInternal(node->children[idx], pred);
        }
        else if (node->children[idx + 1]->key_count >= degree / 2) {
            int succ = getSucc(node, idx);
            node->keys[idx] = succ;
            removeInternal(node->children[idx + 1], succ);
        }

        else {
            merge(node, idx);
            removeInternal(node->children[idx], k);
        }
    }

    int BPlusTree::getPred(BPlusNode* node, int idx) {

        BPlusNode* cur = node->children[idx];
        while (!cur->is_leaf)
            cur = cur->children[cur->key_count];
        return cur->keys[cur->key_count - 1];
    }

    int BPlusTree::getSucc(BPlusNode* node, int idx) {
        BPlusNode* cur = node->children[idx + 1];
        while (!cur->is_leaf)
            cur = cur->children[0];
        return cur->keys[0];
    }

    void BPlusTree::fill(BPlusNode* node, int idx) {
        // Sol kardeşten ödünç alabilir miyiz?
        if (idx != 0 && node->children[idx - 1]->key_count >= degree / 2)
            borrowFromPrev(node, idx);
        
        // Sağ kardeşten ödünç alabilir miyiz?
        else if (idx != node->key_count && node->children[idx + 1]->key_count >= degree / 2)
            borrowFromNext(node, idx);
        
        // Kimseden alamadık, birleştir (Merge)
        else {
            if (idx != node->key_count)
                merge(node, idx);
            else
                merge(node, idx - 1);
        }
    }

    void BPlusTree::borrowFromPrev(BPlusNode* node, int idx) {
        BPlusNode* child = node->children[idx];
        BPlusNode* sibling = node->children[idx - 1];

        // Child'da yer aç (Sağa kaydır)
        for (int i = child->key_count - 1; i >= 0; --i) {
            child->keys[i + 1] = child->keys[i];
            if(child->is_leaf) child->values[i+1] = child->values[i];
        }
        if (!child->is_leaf) {
            for (int i = child->key_count; i >= 0; --i)
                child->children[i + 1] = child->children[i];
        }

        // Babadan anahtarı indir
        child->keys[0] = node->keys[idx - 1];
        if (!child->is_leaf) {
            child->children[0] = sibling->children[sibling->key_count];
        } else {
            // Yapraksa sibling'in son değerini direkt al
            child->keys[0] = sibling->keys[sibling->key_count - 1];
            child->values[0] = sibling->values[sibling->key_count - 1];
        }

        // Babayı güncelle
        node->keys[idx - 1] = sibling->keys[sibling->key_count - 1];

        child->key_count += 1;
        sibling->key_count -= 1;
    }

    void BPlusTree::borrowFromNext(BPlusNode* node, int idx) {
        BPlusNode* child = node->children[idx];
        BPlusNode* sibling = node->children[idx + 1];

        // Sibling'in başındakini Child'ın sonuna ekle
        if (!child->is_leaf) {
            child->keys[child->key_count] = node->keys[idx];
            child->children[child->key_count + 1] = sibling->children[0];
        } else {
            child->keys[child->key_count] = sibling->keys[0];
            child->values[child->key_count] = sibling->values[0];
        }

        // Babayı güncelle
        node->keys[idx] = sibling->keys[0];

        // Sibling'i sola kaydır (Kuyruğu kapat)
        for (int i = 1; i < sibling->key_count; ++i) {
            sibling->keys[i - 1] = sibling->keys[i];
            if(sibling->is_leaf) sibling->values[i-1] = sibling->values[i];
        }
        if (!sibling->is_leaf) {
            for (int i = 1; i <= sibling->key_count; ++i)
                sibling->children[i - 1] = sibling->children[i];
        }

        child->key_count += 1;
        sibling->key_count -= 1;
    }

    void BPlusTree::merge(BPlusNode* node, int idx) {
        BPlusNode* child = node->children[idx];
        BPlusNode* sibling = node->children[idx + 1];
        int t = degree / 2; // Bu örnekte referans olarak

        // İç düğümse, babadan anahtarı (separator) indir
        if (!child->is_leaf) {
            // child->keys'in sonuna babadan gelen anahtarı ekle
            child->keys[child->key_count] = node->keys[idx];
            child->key_count++;
        }

        // Sibling'deki her şeyi Child'a kopyala
        for (int i = 0; i < sibling->key_count; ++i) {
            child->keys[child->key_count] = sibling->keys[i];
            if (child->is_leaf) {
                child->values[child->key_count] = sibling->values[i];
            }
            child->key_count++;
        }
        
        // Çocukları da kopyala (İç düğümse)
        if (!child->is_leaf) {
            for (int i = 0; i <= sibling->key_count; ++i) {
                // child'ın mevcut çocuk sayısından itibaren ekle
                // child->key_count şu an toplam anahtar sayısı oldu,
                // kopyalamaya başlamadan önceki çocuk sayısı üzerinden indekslemeliyiz
                // Ama basit mantıkla: sibling'in çocuklarını child'ın sonuna ekle.
                // Burada sibling->key_count kadar anahtar ekledik, +1 kadar çocuk var.
                // İndeks hesaplaması karışık olabilir, en temizi append mantığıdır.
                // child'ın çocukları [0 ... old_count] idi.
                // şimdi [old_count+1 ... ] sibling'den gelecek.
                // Yukarıdaki döngüden child->key_count arttı, onu düzeltelim:
                // Sibling kopyalamadan önceki anahtar sayısı: child->key_count - sibling->key_count
                int startIdx = child->key_count - sibling->key_count; 
                child->children[startIdx + i] = sibling->children[i];
            }
        }

        // Yaprak zincirini düzelt (Next pointer)
        if (child->is_leaf) {
            child->next = sibling->next;
        }

        // Babadaki boşluğu kapat (Anahtarı ve Pointer'ı sil)
        for (int i = idx + 1; i < node->key_count; ++i) {
            node->keys[i - 1] = node->keys[i];
        }
        for (int i = idx + 2; i <= node->key_count; ++i) {
            node->children[i - 1] = node->children[i];
        }
        
        node->key_count--;

        // Sibling artık boş ve gereksiz, hafızadan sil.
        // Dikkat: Destructor'ı children'ı da siler ama children pointerlarını child'a aktardık.
        // O yüzden sibling->children pointerlarını nullptr yapıp öyle silmeliyiz.
        if(!sibling->is_leaf) sibling->children = nullptr; 
        delete sibling;
    }

    void BPlusTree::print() {
        if (root == nullptr) {
            std::cout << "Tree is empty." << std::endl;
            return;
        }
        std::cout << "\n--- B+ Tree Structure ---" << std::endl;
        printTree(root, 0);
        std::cout << "-------------------------\n" << std::endl;
    }

    void BPlusTree::printTree(BPlusNode* node, int level) {
        if (node == nullptr) return;

        // Seviyeye göre girinti (Indentation) yap
        for (int i = 0; i < level; ++i) std::cout << "    ";

        // Düğüm tipini ve anahtarları yazdır
        if (level == 0) std::cout << "[ROOT] ";
        else if (node->is_leaf) std::cout << "[LEAF] ";
        else std::cout << "[INTERNAL] ";

        std::cout << "[ ";
        for (int i = 0; i < node->key_count; i++) {
            std::cout << node->keys[i] << (i < node->key_count - 1 ? " | " : "");
        }
        std::cout << " ]" << std::endl;

        // Eğer iç düğümse, çocukları için recursive çağır
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; i++) {
                printTree(node->children[i], level + 1);
            }
        }
    }

}
