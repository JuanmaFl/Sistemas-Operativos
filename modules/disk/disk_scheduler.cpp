// modules/disk/disk_scheduler.cpp

#include "disk_scheduler.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cmath>

DiskScheduler::DiskScheduler(int initial_position, DiskAlgorithm algo)
    : current_algorithm(algo),
    current_head_position(initial_position),
    direction(1),
    total_head_movement(0),
    total_requests_served(0) {

    std::cout << "[DISK] Planificador inicializado en cilindro " << initial_position
        << " con algoritmo "
        << (algo == DiskAlgorithm::FCFS ? "FCFS" :
            algo == DiskAlgorithm::SSTF ? "SSTF" : "SCAN")
        << "." << std::endl;
}

void DiskScheduler::add_request(int pid, int cylinder, int current_time) {
    if (cylinder < 0 || cylinder >= TOTAL_CYLINDERS) {
        std::cout << "[DISK ERROR] Cilindro " << cylinder << " fuera de rango (0-"
            << TOTAL_CYLINDERS - 1 << ")." << std::endl;
        return;
    }

    DiskRequest req;
    req.pid = pid;
    req.cylinder = cylinder;
    req.arrival_time = current_time;

    request_queue.push_back(req);

    std::cout << "[DISK] Nueva solicitud: PID " << pid << " → Cilindro " << cylinder
        << " (T=" << current_time << ", Cola: " << request_queue.size() << ")" << std::endl;
}

void DiskScheduler::process_next_request(int current_time) {
    if (request_queue.empty()) {
        std::cout << "[DISK] Cola vacía. Cabezal en reposo en cilindro "
            << current_head_position << "." << std::endl;
        return;
    }

    DiskRequest next_request;

    // Seleccionar siguiente solicitud según algoritmo
    switch (current_algorithm) {
    case DiskAlgorithm::FCFS:
        next_request = select_fcfs();
        break;
    case DiskAlgorithm::SSTF:
        next_request = select_sstf();
        break;
    case DiskAlgorithm::SCAN:
        next_request = select_scan();
        break;
    }

    // Calcular movimiento
    int movement = std::abs(next_request.cylinder - current_head_position);
    total_head_movement += movement;

    std::cout << "[DISK] Atendiendo PID " << next_request.pid
        << ": " << current_head_position << " → " << next_request.cylinder
        << " (Movimiento: " << movement << ", Total: " << total_head_movement << ")"
        << std::endl;

    // Actualizar posición
    current_head_position = next_request.cylinder;
    next_request.completion_time = current_time;

    // Registrar solicitud completada
    completed_requests.push_back(next_request);
    total_requests_served++;
}

// --- ALGORITMOS DE SELECCIÓN ---

DiskRequest DiskScheduler::select_fcfs() {
    // FCFS: Tomar el primero de la cola
    DiskRequest req = request_queue.front();
    request_queue.pop_front();
    return req;
}

DiskRequest DiskScheduler::select_sstf() {
    // SSTF: Buscar el cilindro más cercano
    int min_distance = std::numeric_limits<int>::max();
    auto closest_it = request_queue.begin();

    for (auto it = request_queue.begin(); it != request_queue.end(); ++it) {
        int distance = std::abs(it->cylinder - current_head_position);
        if (distance < min_distance) {
            min_distance = distance;
            closest_it = it;
        }
    }

    DiskRequest req = *closest_it;
    request_queue.erase(closest_it);
    return req;
}

DiskRequest DiskScheduler::select_scan() {
    // SCAN: Buscar en la dirección actual
    DiskRequest selected_req;
    bool found = false;

    // Buscar en la dirección actual
    for (auto it = request_queue.begin(); it != request_queue.end(); ++it) {
        if (direction == 1 && it->cylinder >= current_head_position) {
            if (!found || it->cylinder < selected_req.cylinder) {
                selected_req = *it;
                found = true;
            }
        }
        else if (direction == -1 && it->cylinder <= current_head_position) {
            if (!found || it->cylinder > selected_req.cylinder) {
                selected_req = *it;
                found = true;
            }
        }
    }

    // Si no hay solicitudes en la dirección actual, cambiar dirección
    if (!found) {
        direction *= -1;
        std::cout << "[DISK SCAN] Cambio de dirección → "
            << (direction == 1 ? "ADELANTE" : "ATRÁS") << std::endl;

        // Buscar en la nueva dirección (seleccionar el más cercano)
        for (auto it = request_queue.begin(); it != request_queue.end(); ++it) {
            if (direction == 1 && it->cylinder >= current_head_position) {
                if (!found || it->cylinder < selected_req.cylinder) {
                    selected_req = *it;
                    found = true;
                }
            }
            else if (direction == -1 && it->cylinder <= current_head_position) {
                if (!found || it->cylinder > selected_req.cylinder) {
                    selected_req = *it;
                    found = true;
                }
            }
        }
    }

    // Eliminar de la cola
    auto it = std::find_if(request_queue.begin(), request_queue.end(),
        [&selected_req](const DiskRequest& r) {
            return r.pid == selected_req.pid &&
                r.cylinder == selected_req.cylinder &&
                r.arrival_time == selected_req.arrival_time;
        });

    if (it != request_queue.end()) {
        request_queue.erase(it);
    }

    return selected_req;
}

int DiskScheduler::calculate_seek_distance(int from, int to) const {
    return std::abs(to - from);
}

// --- SETTERS ---

void DiskScheduler::set_algorithm(DiskAlgorithm algo) {
    current_algorithm = algo;
    std::string algo_name = (algo == DiskAlgorithm::FCFS ? "FCFS" :
        algo == DiskAlgorithm::SSTF ? "SSTF" : "SCAN");
    std::cout << "[DISK] Algoritmo cambiado a: " << algo_name << "." << std::endl;
}

void DiskScheduler::set_head_position(int position) {
    if (position >= 0 && position < TOTAL_CYLINDERS) {
        current_head_position = position;
        std::cout << "[DISK] Cabezal reposicionado en cilindro " << position << "." << std::endl;
    }
    else {
        std::cout << "[DISK ERROR] Posición inválida: " << position << std::endl;
    }
}

// --- VISUALIZACIÓN ---

void DiskScheduler::print_status() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     ESTADO DEL PLANIFICADOR DE DISCO" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string algo_name = (current_algorithm == DiskAlgorithm::FCFS ? "FCFS" :
        current_algorithm == DiskAlgorithm::SSTF ? "SSTF" : "SCAN");

    std::cout << "Algoritmo: " << algo_name << std::endl;
    std::cout << "Posición del Cabezal: " << current_head_position << std::endl;
    if (current_algorithm == DiskAlgorithm::SCAN) {
        std::cout << "Dirección SCAN: " << (direction == 1 ? "→ ADELANTE" : "← ATRÁS") << std::endl;
    }
    std::cout << "Solicitudes Pendientes: " << request_queue.size() << std::endl;

    if (!request_queue.empty()) {
        std::cout << "\nCola de Solicitudes:" << std::endl;
        std::cout << std::setw(8) << "PID" << std::setw(12) << "Cilindro"
            << std::setw(12) << "Distancia" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        for (const auto& req : request_queue) {
            int dist = std::abs(req.cylinder - current_head_position);
            std::cout << std::setw(8) << req.pid
                << std::setw(12) << req.cylinder
                << std::setw(12) << dist << std::endl;
        }
    }
    std::cout << "========================================\n" << std::endl;
}

void DiskScheduler::print_stats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     ESTADÍSTICAS DE DISCO" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string algo_name = (current_algorithm == DiskAlgorithm::FCFS ? "FCFS" :
        current_algorithm == DiskAlgorithm::SSTF ? "SSTF" : "SCAN");

    std::cout << "Algoritmo Actual: " << algo_name << std::endl;
    std::cout << "Total de Solicitudes Atendidas: " << total_requests_served << std::endl;
    std::cout << "Movimiento Total del Cabezal: " << total_head_movement << " cilindros" << std::endl;

    if (total_requests_served > 0) {
        double avg_movement = static_cast<double>(total_head_movement) / total_requests_served;
        std::cout << "Movimiento Promedio: " << std::fixed << std::setprecision(2)
            << avg_movement << " cilindros/solicitud" << std::endl;
    }

    std::cout << "Solicitudes Pendientes: " << request_queue.size() << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void DiskScheduler::visualize_disk() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     VISUALIZACIÓN DEL DISCO" << std::endl;
    std::cout << "========================================" << std::endl;

    // Mostrar línea de cilindros en grupos de 20
    const int DISPLAY_WIDTH = 60;
    const int STEP = TOTAL_CYLINDERS / DISPLAY_WIDTH;

    std::cout << "Cilindros: 0";
    for (int i = STEP; i < TOTAL_CYLINDERS; i += (TOTAL_CYLINDERS / 10)) {
        std::cout << std::setw(6) << i;
    }
    std::cout << std::setw(6) << (TOTAL_CYLINDERS - 1) << std::endl;

    // Línea base
    std::cout << "           |";
    for (int i = 1; i < DISPLAY_WIDTH; ++i) {
        std::cout << "-";
    }
    std::cout << "|" << std::endl;

    // Posición del cabezal
    std::cout << "Cabezal:   |";
    int head_pos = (current_head_position * DISPLAY_WIDTH) / TOTAL_CYLINDERS;
    for (int i = 0; i < DISPLAY_WIDTH; ++i) {
        if (i == head_pos) {
            std::cout << "▼";
        }
        else {
            std::cout << " ";
        }
    }
    std::cout << "|  [" << current_head_position << "]" << std::endl;

    // Solicitudes pendientes
    if (!request_queue.empty()) {
        std::cout << "Solicitudes:|";
        std::vector<bool> has_request(DISPLAY_WIDTH, false);

        for (const auto& req : request_queue) {
            int pos = (req.cylinder * DISPLAY_WIDTH) / TOTAL_CYLINDERS;
            has_request[pos] = true;
        }

        for (int i = 0; i < DISPLAY_WIDTH; ++i) {
            std::cout << (has_request[i] ? "*" : " ");
        }
        std::cout << "|" << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}