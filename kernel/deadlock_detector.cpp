#include "deadlock_detector.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

DeadlockDetector::DeadlockDetector() : num_processes(0) {
    state.available = { 3, 3, 2 };
    std::cout << "[DEADLOCK] Detector inicializado. Recursos: [3, 3, 2]" << std::endl;
}

void DeadlockDetector::initialize_process(int pid, const std::vector<int>& max_resources) {
    if (max_resources.size() != NUM_RESOURCE_TYPES) {
        std::cout << "[DEADLOCK ERROR] Vector de máximos inválido." << std::endl;
        return;
    }

    while (state.max_demand.size() <= static_cast<size_t>(pid)) {
        state.max_demand.push_back(std::vector<int>(NUM_RESOURCE_TYPES, 0));
        state.allocation.push_back(std::vector<int>(NUM_RESOURCE_TYPES, 0));
        state.need.push_back(std::vector<int>(NUM_RESOURCE_TYPES, 0));
    }

    state.max_demand[pid] = max_resources;
    state.need[pid] = max_resources;
    num_processes = std::max(num_processes, pid + 1);

    std::cout << "[DEADLOCK] Proceso " << pid << " inicializado. Max: [";
    for (size_t i = 0; i < max_resources.size(); i++) {
        std::cout << max_resources[i];
        if (i < max_resources.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

bool DeadlockDetector::request_resources(int pid, const std::vector<int>& request) {
    total_detections++;

    if (pid >= num_processes || request.size() != NUM_RESOURCE_TYPES) {
        std::cout << "[DEADLOCK ERROR] Solicitud inválida." << std::endl;
        return false;
    }

    for (size_t i = 0; i < NUM_RESOURCE_TYPES; i++) {
        if (request[i] > state.need[pid][i]) {
            std::cout << "[DEADLOCK] ERROR: PID " << pid << " excede su máximo declarado." << std::endl;
            return false;
        }
        if (request[i] > state.available[i]) {
            std::cout << "[DEADLOCK] PID " << pid << " debe esperar. Recursos insuficientes." << std::endl;
            return false;
        }
    }

    for (size_t i = 0; i < NUM_RESOURCE_TYPES; i++) {
        state.available[i] -= request[i];
        state.allocation[pid][i] += request[i];
        state.need[pid][i] -= request[i];
    }

    if (is_safe_state()) {
        safe_states++;
        std::cout << "[BANKER] ✓ Estado SEGURO. Solicitud concedida a PID " << pid << std::endl;
        return true;
    }
    else {
        unsafe_states++;
        std::cout << "[BANKER] ✗ Estado INSEGURO. Revertir asignación." << std::endl;

        for (size_t i = 0; i < NUM_RESOURCE_TYPES; i++) {
            state.available[i] += request[i];
            state.allocation[pid][i] -= request[i];
            state.need[pid][i] += request[i];
        }
        return false;
    }
}

void DeadlockDetector::release_resources(int pid, const std::vector<int>& release) {
    if (pid >= num_processes) return;

    // VALIDACIÓN: No liberar más de lo asignado
    for (size_t i = 0; i < NUM_RESOURCE_TYPES; i++) {
        if (release[i] > state.allocation[pid][i]) {
            std::cout << "[DEADLOCK] ERROR: PID " << pid << " intenta liberar más recursos de los asignados." << std::endl;
            return;
        }
    }

    for (size_t i = 0; i < NUM_RESOURCE_TYPES; i++) {
        state.allocation[pid][i] -= release[i];
        state.available[i] += release[i];
        state.need[pid][i] += release[i];
    }

    std::cout << "[DEADLOCK] PID " << pid << " liberó recursos. Disponibles: [";
    for (size_t i = 0; i < state.available.size(); i++) {
        std::cout << state.available[i];
        if (i < state.available.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

bool DeadlockDetector::is_safe_state() {
    std::vector<int> work = state.available;
    std::vector<bool> finish(num_processes, false);
    std::vector<int> sequence;

    return banker_algorithm(work, finish, sequence);
}

std::vector<int> DeadlockDetector::find_safe_sequence() {
    std::vector<int> work = state.available;
    std::vector<bool> finish(num_processes, false);
    std::vector<int> sequence;

    banker_algorithm(work, finish, sequence);
    return sequence;
}

bool DeadlockDetector::banker_algorithm(const std::vector<int>& work, std::vector<bool>& finish, std::vector<int>& sequence) {
    std::vector<int> current_work = work;
    int completed = 0;

    while (completed < num_processes) {
        bool found = false;

        for (int i = 0; i < num_processes; i++) {
            if (!finish[i]) {
                bool can_finish = true;
                for (size_t j = 0; j < NUM_RESOURCE_TYPES; j++) {
                    if (state.need[i][j] > current_work[j]) {
                        can_finish = false;
                        break;
                    }
                }

                if (can_finish) {
                    for (size_t j = 0; j < NUM_RESOURCE_TYPES; j++) {
                        current_work[j] += state.allocation[i][j];
                    }
                    finish[i] = true;
                    sequence.push_back(i);
                    completed++;
                    found = true;
                }
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

void DeadlockDetector::detect_deadlock() {
    if (is_safe_state()) {
        std::vector<int> seq = find_safe_sequence();
        std::cout << "[DEADLOCK] Sistema en estado SEGURO. Secuencia: [";
        for (size_t i = 0; i < seq.size(); i++) {
            std::cout << "P" << seq[i];
            if (i < seq.size() - 1) std::cout << " → ";
        }
        std::cout << "]" << std::endl;
    }
    else {
        std::cout << "[DEADLOCK] ⚠️ DEADLOCK DETECTADO. Sistema en estado INSEGURO." << std::endl;
    }
}

void DeadlockDetector::print_resource_state() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "      ESTADO DE RECURSOS (Banker)" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "Disponibles: [";
    for (size_t i = 0; i < state.available.size(); i++) {
        std::cout << state.available[i];
        if (i < state.available.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n" << std::endl;

    std::cout << std::setw(5) << "PID"
        << std::setw(15) << "Asignado"
        << std::setw(15) << "Máximo"
        << std::setw(15) << "Necesita" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    for (int i = 0; i < num_processes; i++) {
        std::cout << std::setw(5) << i;

        std::cout << std::setw(15) << "[";
        for (size_t j = 0; j < NUM_RESOURCE_TYPES; j++) {
            std::cout << state.allocation[i][j];
            if (j < NUM_RESOURCE_TYPES - 1) std::cout << ",";
        }
        std::cout << "]";

        std::cout << std::setw(12) << "[";
        for (size_t j = 0; j < NUM_RESOURCE_TYPES; j++) {
            std::cout << state.max_demand[i][j];
            if (j < NUM_RESOURCE_TYPES - 1) std::cout << ",";
        }
        std::cout << "]";

        std::cout << std::setw(12) << "[";
        for (size_t j = 0; j < NUM_RESOURCE_TYPES; j++) {
            std::cout << state.need[i][j];
            if (j < NUM_RESOURCE_TYPES - 1) std::cout << ",";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}

void DeadlockDetector::print_deadlock_stats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "      ESTADÍSTICAS DE DEADLOCK" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total Solicitudes: " << total_detections << std::endl;
    std::cout << "Estados Seguros: " << safe_states << std::endl;
    std::cout << "Estados Inseguros: " << unsafe_states << std::endl;

    if (total_detections > 0) {
        double safe_ratio = (static_cast<double>(safe_states) / total_detections) * 100.0;
        std::cout << "Tasa de Seguridad: " << std::fixed << std::setprecision(2)
            << safe_ratio << "%" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}