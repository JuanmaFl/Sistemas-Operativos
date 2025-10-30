// modules/mem/page_table.h
#ifndef MEM_PAGE_TABLE_H
#define MEM_PAGE_TABLE_H

#include <vector>
#include <stdexcept> // Para std::out_of_range

// --- Configuración (Asegúrate que estas constantes están definidas) ---
const int VIRTUAL_PAGES = 32;

// ----------------------------------------------------
// Estructura: Page Table Entry (PTE)
// ----------------------------------------------------
struct PageTableEntry {
    int frame_number = -1;
    bool present_bit = false;
    bool referenced_bit = false;
    // ⭐️ Campo clave para LRU ⭐️
    int last_access_time = 0;
};

// ----------------------------------------------------
// Clase: PageTable
// ----------------------------------------------------
class PageTable {
private:
    std::vector<PageTableEntry> entries;
public:
    PageTable() : entries(VIRTUAL_PAGES) {}

    // ¡La implementación de esta función es necesaria para que el código compile!
    PageTableEntry& get_pte(int virtual_page_number) {
        if (virtual_page_number < 0 || virtual_page_number >= VIRTUAL_PAGES) {
            throw std::out_of_range("Numero de página virtual fuera de rango.");
        }
        return entries[virtual_page_number];
    }
};

#endif // MEM_PAGE_TABLE_H