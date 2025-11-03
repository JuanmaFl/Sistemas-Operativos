#include "memory_manager.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>

MemoryManager::MemoryManager() {
    physical_memory.resize(FRAME_COUNT);
    for (int i = 0; i < FRAME_COUNT; ++i) {
        physical_memory[i].frame_id = i;
        physical_memory[i].pid = -1;
        physical_memory[i].page_number = -1;
        physical_memory[i].load_time = 0;
        physical_memory[i].last_access_time = 0;
    }
}

int MemoryManager::get_free_frame() {
    for (int i = 0; i < FRAME_COUNT; ++i) {
        if (physical_memory[i].pid == -1) {
            return i;
        }
    }
    return -1;
}

int MemoryManager::find_frame_to_replace(PageTable& page_table, int pid, int current_time) {
    if (current_algorithm == ReplacementAlgorithm::FIFO) {
        if (fifo_queue.empty()) { return -1; }
        int victim_frame = fifo_queue.front();
        fifo_queue.pop_front();
        return victim_frame;
    }
    else if (current_algorithm == ReplacementAlgorithm::LRU) {
        int victim_frame = -1;
        int oldest_time = std::numeric_limits<int>::max();

        for (int i = 0; i < FRAME_COUNT; ++i) {
            if (physical_memory[i].pid != -1) {
                if (physical_memory[i].last_access_time < oldest_time) {
                    oldest_time = physical_memory[i].last_access_time;
                    victim_frame = i;
                }
            }
        }
        return victim_frame;
    }
    else if (current_algorithm == ReplacementAlgorithm::PFF) {
        return find_frame_pff(pid, current_time);
    }
    return -1;
}

int MemoryManager::find_frame_pff(int pid, int current_time) {
    int victim_frame = -1;
    int oldest_time = std::numeric_limits<int>::max();

    for (int i = 0; i < FRAME_COUNT; ++i) {
        if (physical_memory[i].pid == pid) {
            if (physical_memory[i].last_access_time < oldest_time) {
                oldest_time = physical_memory[i].last_access_time;
                victim_frame = i;
            }
        }
    }

    if (victim_frame == -1) {
        for (int i = 0; i < FRAME_COUNT; ++i) {
            if (physical_memory[i].pid != -1) {
                if (physical_memory[i].last_access_time < oldest_time) {
                    oldest_time = physical_memory[i].last_access_time;
                    victim_frame = i;
                }
            }
        }
    }

    return victim_frame;
}

void MemoryManager::update_pff_state(int pid, int current_time, bool page_fault) {
    PFFState& state = pff_states[pid];

    if (page_fault) {
        state.page_faults++;

        int time_since_last = current_time - state.last_fault_time;
        if (time_since_last > 0 && time_since_last <= PFF_WINDOW) {
            state.fault_rate = 1.0 / time_since_last;
        }

        state.last_fault_time = current_time;

        if (state.fault_rate > PFF_UPPER_THRESHOLD) {
            state.allocated_frames++;
            std::cout << "[PFF] Alta tasa de fallos (PID " << pid
                << "): Incrementar marcos a " << state.allocated_frames << std::endl;
        }
    }
    else {
        if (current_time - state.last_fault_time > PFF_WINDOW) {
            state.fault_rate = 0.0;

            if (state.allocated_frames > 1 && state.fault_rate < PFF_LOWER_THRESHOLD) {
                state.allocated_frames--;
                std::cout << "[PFF] Baja tasa de fallos (PID " << pid
                    << "): Decrementar marcos a " << state.allocated_frames << std::endl;
            }
        }
    }
}

void MemoryManager::evict_frame(int victim_frame_id, int current_time, PageTable& page_table) {
    Frame& victim_frame = physical_memory[victim_frame_id];

    PageTableEntry& old_pte = page_table.get_pte(victim_frame.page_number);
    old_pte.present_bit = false;
    old_pte.frame_number = -1;
    old_pte.referenced_bit = false;
    old_pte.last_access_time = 0;

    std::string algo_name = get_algorithm_name();

    std::cout << "[MEM] Reemplazo: Marco " << victim_frame_id
        << " (Pág " << victim_frame.page_number
        << " de Proc " << victim_frame.pid << ") desalojado (" << algo_name << ")." << std::endl;

    victim_frame.pid = -1;
    victim_frame.page_number = -1;
    victim_frame.load_time = 0;
    victim_frame.last_access_time = 0;
}

void MemoryManager::access_memory(int pid, int virtual_address, PageTable& page_table, int current_time) {
    total_accesses++;

    int virtual_page_number = virtual_address / FRAME_SIZE;

    if (virtual_page_number >= VIRTUAL_PAGES || virtual_page_number < 0) {
        std::cout << "[MEM] Error: Dirección virtual " << virtual_address << " fuera de rango (Proc " << pid << ")." << std::endl;
        return;
    }

    PageTableEntry& pte = page_table.get_pte(virtual_page_number);

    if (pte.present_bit) {
        total_hits++;
        pte.last_access_time = current_time;
        pte.referenced_bit = true;
        physical_memory[pte.frame_number].last_access_time = current_time;

        if (current_algorithm == ReplacementAlgorithm::PFF) {
            update_pff_state(pid, current_time, false);
        }

        std::cout << "[MEM] HIT: Proc " << pid << ", Pág " << virtual_page_number
            << " está en Marco " << pte.frame_number << "." << std::endl;
    }
    else {
        total_faults++;
        std::cout << "[MEM] PAGE FAULT (T=" << current_time << "): Proc " << pid
            << " solicita Pág " << virtual_page_number << std::endl;

        if (current_algorithm == ReplacementAlgorithm::PFF) {
            update_pff_state(pid, current_time, true);
        }

        int frame_to_use = get_free_frame();

        if (frame_to_use == -1) {
            frame_to_use = find_frame_to_replace(page_table, pid, current_time);
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

        Frame& new_frame = physical_memory[frame_to_use];
        new_frame.pid = pid;
        new_frame.page_number = virtual_page_number;
        new_frame.load_time = current_time;
        new_frame.last_access_time = current_time;

        if (current_algorithm == ReplacementAlgorithm::FIFO) {
            fifo_queue.push_back(frame_to_use);
        }

        pte.frame_number = frame_to_use;
        pte.present_bit = true;
        pte.referenced_bit = true;
        pte.last_access_time = current_time;
    }
}

void MemoryManager::set_replacement_algorithm(ReplacementAlgorithm algo) {
    current_algorithm = algo;
    std::cout << "[MEM] Algoritmo de reemplazo cambiado a: " << get_algorithm_name() << "." << std::endl;
}

std::string MemoryManager::get_algorithm_name() const {
    switch (current_algorithm) {
    case ReplacementAlgorithm::FIFO: return "FIFO";
    case ReplacementAlgorithm::LRU: return "LRU";
    case ReplacementAlgorithm::PFF: return "PFF";
    default: return "UNKNOWN";
    }
}

void MemoryManager::print_memory_status() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "        ESTADO DE MEMORIA FÍSICA" << std::endl;
    std::cout << "Algoritmo: " << get_algorithm_name() << std::endl;
    std::cout << "========================================" << std::endl;

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
            std::cout << std::setw(6) << frame.pid
                << std::setw(8) << frame.page_number
                << std::setw(10) << frame.load_time
                << std::setw(10) << frame.last_access_time;
        }
        else {
            std::cout << std::setw(6) << "-"
                << std::setw(8) << "-"
                << std::setw(10) << "-"
                << std::setw(10) << "-";
        }
        std::cout << std::endl;
    }

    std::cout << "------------------------------------------" << std::endl;
}

void MemoryManager::print_memory_stats() const {
    double hit_ratio = 0.0;
    if (total_accesses > 0) {
        hit_ratio = static_cast<double>(total_hits) / total_accesses;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "        ESTADÍSTICAS DE MEMORIA" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Algoritmo: " << get_algorithm_name() << std::endl;
    std::cout << "Total Accesos:      " << total_accesses << std::endl;
    std::cout << "Total Aciertos:     " << total_hits << std::endl;
    std::cout << "Total Fallos:       " << total_faults << std::endl;
    std::cout << "Tasa de Aciertos: " << std::fixed << std::setprecision(2) << hit_ratio * 100.0 << "%" << std::endl;

    if (current_algorithm == ReplacementAlgorithm::PFF && !pff_states.empty()) {
        std::cout << "\n--- Estado PFF por Proceso ---" << std::endl;
        for (const auto& pair : pff_states) {
            std::cout << "PID " << pair.first << ": Marcos=" << pair.second.allocated_frames
                << ", Fallos=" << pair.second.page_faults
                << ", Tasa=" << std::fixed << std::setprecision(3) << pair.second.fault_rate << std::endl;
        }
    }

    std::cout << "========================================" << std::endl;
}