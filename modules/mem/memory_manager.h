#ifndef MEM_MEMORY_MANAGER_H
#define MEM_MEMORY_MANAGER_H

#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>
#include <limits> // Para std::numeric_limits
#include "page_table.h" // Necesita saber que es PageTable


const int FRAME_COUNT = 16;  // Total de marcos (paginas fisicas)
const int FRAME_SIZE = 256;  // Tamano de cada marco en bytes (el mismo que el tamano de pagina virtual)


// ENUMS Y ESTRUCTURAS


// Enum para el Algoritmo de Reemplazo
enum class ReplacementAlgorithm {
    FIFO,
    LRU
};

// ⭐️ ESTRUCTURA Frame CORREGIDA ⭐️
// Se añade last_access_time, esencial para LRU y consistente con el .cpp.
struct Frame {
    int frame_id;
    int pid = -1;       // ID del proceso que ocupa este marco (-1 significa libre)
    int page_number = -1; // Numero de pagina virtual asignada a este marco
    int load_time = 0;  // Tiempo de carga (para FIFO)
    int last_access_time = 0; // ⭐️ NUEVO: Último tiempo de acceso (para LRU) ⭐️
};


// CLASE GESTOR DE MEMORIA

class MemoryManager {
private:
    std::vector<Frame> physical_memory; // Array que representa la memoria fisica
    std::deque<int> fifo_queue;          // Cola para el algoritmo de reemplazo FIFO (contiene frame_id's)
    ReplacementAlgorithm current_algorithm = ReplacementAlgorithm::FIFO; // Algoritmo actual 

    // Miembros para estadísticas
    int total_accesses = 0;
    int total_hits = 0;
    int total_faults = 0;

public:
    // Constructor
    MemoryManager();

    // Funcion principal para el acceso a memoria (llamada desde kernel/core.cpp)
    void access_memory(int pid, int virtual_address, PageTable& page_table, int current_time);

    // ⭐️ CORRECCIÓN/VERIFICACIÓN: Debe ser const para ser correcta ⭐️
    // Permite que se llame desde funciones const del Kernel (como en core.cpp)
    void print_memory_status() const;
    void print_memory_stats() const;

    // Setter para cambiar el algoritmo
    void set_replacement_algorithm(ReplacementAlgorithm algo) {
        current_algorithm = algo;
        // La impresión se puede dejar aquí o mover al .cpp si es más larga.
        std::cout << "[MEM] Algoritmo de reemplazo cambiado a: "
            << (algo == ReplacementAlgorithm::LRU ? "LRU" : "FIFO") << "." << std::endl;
    }

private:
    // Funciones auxiliares privadas
    int get_free_frame();
    int find_frame_to_replace(PageTable& page_table);
    void evict_frame(int victim_frame_id, int current_time, PageTable& page_table);
};

#endif