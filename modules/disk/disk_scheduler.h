// modules/disk/disk_scheduler.h
#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H

#include <vector>
#include <deque>
#include <string>

// Configuración del disco
const int TOTAL_CYLINDERS = 200;  // Cilindros 0-199

// Algoritmos de planificación de disco
enum class DiskAlgorithm {
    FCFS,   // First-Come, First-Served
    SSTF,   // Shortest Seek Time First
    SCAN    // Elevator Algorithm
};

// Estructura para una solicitud de disco
struct DiskRequest {
    int pid;            // ID del proceso solicitante
    int cylinder;       // Cilindro solicitado
    int arrival_time;   // Tiempo de llegada
    int completion_time = -1; // Tiempo de completación
};

class DiskScheduler {
private:
    DiskAlgorithm current_algorithm;
    int current_head_position;      // Posición actual del cabezal
    int direction;                  // Para SCAN: 1 = hacia adelante, -1 = hacia atrás
    std::deque<DiskRequest> request_queue;
    std::vector<DiskRequest> completed_requests;

    // Métricas
    int total_head_movement;
    int total_requests_served;

public:
    DiskScheduler(int initial_position = 50, DiskAlgorithm algo = DiskAlgorithm::FCFS);

    // Operaciones principales
    void add_request(int pid, int cylinder, int current_time);
    void process_next_request(int current_time);

    // Setters
    void set_algorithm(DiskAlgorithm algo);
    void set_head_position(int position);

    // Getters
    int get_head_position() const { return current_head_position; }
    int get_total_movement() const { return total_head_movement; }
    int get_pending_requests() const { return request_queue.size(); }

    // Visualización y estadísticas
    void print_status() const;
    void print_stats() const;
    void visualize_disk() const;

private:
    // Funciones auxiliares por algoritmo
    DiskRequest select_fcfs();
    DiskRequest select_sstf();
    DiskRequest select_scan();

    int calculate_seek_distance(int from, int to) const;
};

#endif // DISK_SCHEDULER_H