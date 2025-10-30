#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <iostream>
#include <vector>
#include <string>
#include "../modules/mem/page_table.h"

// Enum para el estado del proceso
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

// Clase PCB (Process Control Block)
class Process {
public:
    int id;
    ProcessState state;
    int arrival_time;
    int total_burst_time;  // Tiempo total de ráfaga (CPU)
    int remaining_time;    // Tiempo de ráfaga restante (para el planificador)
    int wait_time;         // Tiempo total en cola READY
    int turnaround_time;   // Tiempo de retorno (finalización - llegada)
    int time_on_cpu;       // Tiempo que lleva corriendo en este quantum (para RR)
    PageTable page_table;

    // std::vector<int> io_requests; // Se puede añadir más tarde para simular E/S

    /**
     * @brief Constructor del Bloque de Control de Proceso (PCB).
     */
    Process(int pid, int burst, int arrival = 0)
        : id(pid),
        state(ProcessState::NEW),
        arrival_time(arrival),
        total_burst_time(burst),
        remaining_time(burst),
        wait_time(0),
        turnaround_time(0),
        time_on_cpu(0) // Inicializa el contador de quantum
    {
    }

    /**
     * @brief Simula un ciclo de CPU. Decrementa el tiempo restante y cuenta el tiempo en CPU.
     */
    void run_tick() {
        if (remaining_time > 0) {
            remaining_time--;
            time_on_cpu++; // Incrementa el tiempo en el quantum actual
        }
    }

    /**
     * @brief Resetea el contador de tiempo en CPU (al ser expropiado o despachado).
     */
    void reset_quantum_counter() {
        time_on_cpu = 0;
    }
};

#endif // KERNEL_PROCESS_H