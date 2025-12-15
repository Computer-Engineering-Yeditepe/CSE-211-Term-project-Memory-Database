#ifndef JOINMANAGER_HPP
#define JOINMANAGER_HPP

#include "../core/Table.hpp"

class JoinManager {
public:
    /**
     * @brief Hash Join Algoritması
     * * @param t1 Birinci Tablo (Hash tablosuna yüklenecek olan - Build Phase)
     * @param col1Index t1'deki birleşme sütununun indisi
     * @param t2 İkinci Tablo (Taranacak olan - Probe Phase)
     * @param col2Index t2'deki birleşme sütununun indisi
     * @return Table* Birleştirilmiş yeni tablo pointer'ı döner
     */
    static Table* hashJoin(Table* t1, int col1Index, Table* t2, int col2Index);
};

#endif