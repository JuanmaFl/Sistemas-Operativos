// modules/mem/memory_manager.cpp 

#include "memory_manager.h"
#include <iostream>
#include <iomanip>
#include <string> 
#include <limits> 
#include <algorithm> 

// CONSTRUCTOR


MemoryManager::MemoryManager() {
    // Inicializar marcos
    physical_memory.resize(FRAME_COUNT);
    for (int i = 0; i < FRAME_COUNT; ++i) {
        physical_memory[i].frame_id = i;
        physical_memory[i].pid = -1;
        physical_memory[i].page_number = -1;
        physical_memory[i].load_time = 0;
        physical_memory[i].last_access_time = 0;
    }
}

// FUNCIONES AUXILIARES DE REEMPLAZO


/** Busca el primer marco libre, retorna -1 si no hay. */
int MemoryManager::get_free_frame() {
    for (int i = 0; i < FRAME_COUNT; ++i) {
        if (physical_memory[i].pid == -1) {
            return i;
        }
    }
    return -1; // No hay marco libre
}

/** Implementa el algoritmo de reemplazo (FIFO o LRU), retornando el ID del marco víctima. */
int MemoryManager::find_frame_to_replace(PageTable& page_table) {
    if (current_algorithm == ReplacementAlgorithm::FIFO) {
        // --- Lógica FIFO ---
        if (fifo_queue.empty()) { return -1; }
        int victim_frame = fifo_queue.front();
        fifo_queue.pop_front();
        return victim_frame;
    }
    else if (current_algorithm == ReplacementAlgorithm::LRU) {
        // --- Lógica LRU (Least Recently Used) ---
        int victim_frame = -1;
        int oldest_time = std::numeric_limits<int>::max();

        for (int i = 0; i < FRAME_COUNT; ++i) {
            if (physical_memory[i].pid != -1) {
                // Aquí usamos el last_access_time almacenado en el frame
                if (physical_memory[i].last_access_time < oldest_time) {
                    oldest_time = physical_memory[i].last_access_time;
                    victim_frame = i;
                }
            }
        }
        return victim_frame;
    }
    return -1; // Algoritmo no reconocido
}

/** Función auxiliar que simula el desalojo y limpia la entrada del marco. */
void MemoryManager::evict_frame(int victim_frame_id, int current_time, PageTable& page_table) {
    Frame& victim_frame = physical_memory[victim_frame_id];

    // Invalidar la PTE del proceso propietario
    PageTableEntry& old_pte = page_table.get_pte(victim_frame.page_number);
    old_pte.present_bit = false;
    old_pte.frame_number = -1;
    old_pte.referenced_bit = false;
    old_pte.last_access_time = 0;

    std::string algo_name = (current_algorithm == ReplacementAlgorithm::LRU) ? "LRU" : "FIFO";

    std::cout << "[MEM] 🔄 Reemplazo: Marco " << victim_frame_id
        << " (Pág " << victim_frame.page_number
        << " de Proc " << victim_frame.pid << ") desalojado (" << algo_name << ")." << std::endl;

    // Limpiar el marco físico
    victim_frame.pid = -1;
    victim_frame.page_number = -1;
    victim_frame.load_time = 0;
    victim_frame.last_access_time = 0;
}


// FUNCIÓN PRINCIPAL DE ACCESO


void MemoryManager::access_memory(int pid, int virtual_address, PageTable& page_table, int current_time) {
    total_accesses++;

    int virtual_page_number = virtual_address / FRAME_SIZE;

    if (virtual_page_number >= VIRTUAL_PAGES || virtual_page_number < 0) {
        std::cout << "[MEM] Error: Dirección virtual " << virtual_address << " fuera de rango (Proc " << pid << ")." << std::endl;
        return;
    }

    PageTableEntry& pte = page_table.get_pte(virtual_page_number);

    if (pte.present_bit) {
        // HIT
        total_hits++;
        pte.last_access_time = current_time;
        pte.referenced_bit = true;

        physical_memory[pte.frame_number].last_access_time = current_time;

        std::cout << "[MEM] HIT: Proc " << pid << ", Pág " << virtual_page_number
            << " está en Marco " << pte.frame_number << "." << std::endl;

    }
    else {
        // PAGE FAULT
        total_faults++;
        std::cout << "[MEM] PAGE FAULT (T=" << current_time << "): Proc " << pid
            << " solicita Pág " << virtual_page_number << std::endl;

        int frame_to_use = get_free_frame();

        if (frame_to_use == -1) {
            // Reemplazo necesario
            frame_to_use = find_frame_to_replace(page_table);
            if (frame_to_use != -1) {
                evict_frame(frame_to_use, current_time, page_table);
            }
            else {
                std::cout << "[MEM] ERROR CRÍTICO: No se pudo encontrar marco para reemplazo." << std::endl;
                return;
            }
        }
        else {
            std::cout << "[MEM] Asignando nuevo Marco libre: " << frame_to_use << std::endl;
        }

        // Cargar la nueva página
        Frame& new_frame = physical_memory[frame_to_use];
        new_frame.pid = pid;
        new_frame.page_number = virtual_page_number;
        new_frame.load_time = current_time;
        new_frame.last_access_time = current_time;

        if (current_algorithm == ReplacementAlgorithm::FIFO) {
            fifo_queue.push_back(frame_to_use);
        }

        // Actualizar la PTE
        pte.frame_number = frame_to_use;
        pte.present_bit = true;
        pte.referenced_bit = true;
        pte.last_access_time = current_time;
    }
}

// VISUALIZACIÓN Y ESTADÍSTICAS


// DEFINICIÓN CON 'const'
void MemoryManager::print_memory_status() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "        ESTADO DE MEMORIA FÍSICA" << std::endl;
    std::cout << "Algoritmo: " << (current_algorithm == ReplacementAlgorithm::LRU ? "LRU" : "FIFO") << std::endl;
    std::cout << "========================================" << std::endl;

    // Encabezado de la tabla
    std::cout << std::setw(5) << "Marco"
        << std::setw(8) << "Estado"
        << std::setw(6) << "PID"
        << std::setw(8) << "Pág."
        << std::setw(10) << "T. Carga"
        << std::setw(10) << "T. Últ.A."
        << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    for (int i = 0; i < FRAME_COUNT; ++i) {
        const Frame& frame = physical_memory[i];

        std::string state_str = (frame.pid != -1) ? "OCUPADO" : "LIBRE";

        std::cout << std::setw(5) << i
            << std::setw(8) << state_str;

        if (frame.pid != -1) {
            // Imprimir valores numéricos
            std::cout << std::setw(6) << frame.pid
                << std::setw(8) << frame.page_number
                << std::setw(10) << frame.load_time
                << std::setw(10) << frame.last_access_time;
        }
        else {
            // Marcos libres
            std::cout << std::setw(6) << "-"
                << std::setw(8) << "-"
                << std::setw(10) << "-"
                << std::setw(10) << "-";
        }
        std::cout << std::endl;
    }

    std::cout << "------------------------------------------" << std::endl;
}

// DEFINICIÓN CON 'const'
void MemoryManager::print_memory_stats() const {
    double hit_ratio = 0.0;
    if (total_accesses > 0) {
        hit_ratio = static_cast<double>(total_hits) / total_accesses;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "        ESTADÍSTICAS DE MEMORIA" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Algoritmo: " << (current_algorithm == ReplacementAlgorithm::LRU ? "LRU" : "FIFO") << std::endl;
    std::cout << "Total Accesos:      " << total_accesses << std::endl;
    std::cout << "Total Aciertos:     " << total_hits << std::endl;
    std::cout << "Total Fallos:       " << total_faults << std::endl;
    std::cout << "Tasa de Aciertos: " << std::fixed << std::setprecision(2) << hit_ratio * 100.0 << "%" << std::endl;
    std::cout << "========================================" << std::endl;
}