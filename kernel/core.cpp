#include "core.h"
#include <iostream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <memory>
#include <string>


// CONSTRUCTOR / DESTRUCTOR


KernelSimulator::KernelSimulator()
    : current_time(0) {
    // Inicializar el Scheduler con un quantum de 3 ticks y Round Robin
    scheduler = std::make_unique<Scheduler>(3, SchedulerAlgorithm::ROUND_ROBIN);
    // ⭐️ Inicialización Correcta del Gestor de Memoria
    memory_manager = std::make_unique<MemoryManager>();
    std::cout << "========================================" << std::endl;
    std::cout << " Simulador de Nucleo (Kernel-Sim) v0.1" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[SCHEDULER] Inicializado con algoritmo RR (Quantum: 3)." << std::endl; // Información de inicio agregada
    std::cout << "Kernel Simulator inicializado." << std::endl;
}

KernelSimulator::~KernelSimulator() {
    std::cout << "Kernel Simulator finalizado." << std::endl;
}


// LÓGICA PRINCIPAL (run_tick)


void KernelSimulator::run_tick() {
    current_time++;
    std::cout << "\n[ T=" << current_time << " ] -- Ciclo de reloj --" << std::endl;

    // Ejecutar el tick del scheduler (pasando el tiempo actual)
    scheduler->tick(current_time);
}

void KernelSimulator::run_cycles(int n) {
    for (int i = 0; i < n; ++i) {
        run_tick();
    }
}


// IMPLEMENTACIÓN DE COMANDOS DE PLANIFICACIÓN


// Esta definición es necesaria porque quitamos la inline de core.h
void KernelSimulator::set_quantum(int quantum) {
    if (scheduler) {
        scheduler->set_quantum(quantum);
    }
}

void KernelSimulator::print_stats() {
    double total_wait_time = 0.0;
    double total_turnaround_time = 0.0;
    int terminated_count = 0;

    for (const auto& p : all_processes) {
        if (p->state == ProcessState::TERMINATED) {
            total_wait_time += p->wait_time;
            total_turnaround_time += p->turnaround_time;
            terminated_count++;
        }
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "             Métricas de Simulación" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "Tiempo Actual del Kernel: " << current_time << std::endl;
    std::cout << "Total de Procesos Creados: " << all_processes.size() << std::endl;
    std::cout << "Procesos Terminados: " << terminated_count << std::endl;

    if (terminated_count > 0) {
        double avg_wait = total_wait_time / terminated_count;
        double avg_turnaround = total_turnaround_time / terminated_count;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Tiempo de Espera Promedio: " << avg_wait << std::endl;
        std::cout << "Tiempo de Retorno Promedio: " << avg_turnaround << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
    else {
        std::cout << "No hay procesos terminados para calcular promedios." << std::endl;
    }

    // Llama a las estadísticas de memoria
    memory_manager->print_memory_stats();
}

void KernelSimulator::new_process(int burst) {
    int new_id = all_processes.size() + 1;
    auto p = std::make_shared<Process>(new_id, burst, current_time);
    all_processes.push_back(p);

    // Añadir el proceso al planificador (lo pone en READY)
    scheduler->add_process(p);
    std::cout << "[INFO] Nuevo proceso creado: ID " << new_id
        << " (Burst: " << burst << ", T. Llegada: " << current_time << ")" << std::endl;
}

void KernelSimulator::list_processes() {
    // ... (cuerpo de list_processes sin cambios) ...
    std::cout << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << std::setw(5) << "ID"
        << std::setw(12) << "ESTADO"
        << std::setw(10) << "BURST"
        << std::setw(10) << "REM."
        << std::setw(10) << "WAIT"
        << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    for (const auto& p : all_processes) {
        std::string state_str;
        switch (p->state) {
        case ProcessState::NEW: state_str = "NEW"; break;
        case ProcessState::READY: state_str = "READY"; break;
        case ProcessState::RUNNING: state_str = "RUNNING"; break;
        case ProcessState::WAITING: state_str = "WAITING"; break;
        case ProcessState::TERMINATED: state_str = "TERMINATED"; break;
        }

        std::cout << std::setw(5) << p->id
            << std::setw(12) << state_str
            << std::setw(10) << p->total_burst_time
            << std::setw(10) << p->remaining_time
            << std::setw(10) << p->wait_time
            << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;
}


bool KernelSimulator::kill_process(int pid) {
    // Busca el proceso en la lista global de procesos
    std::shared_ptr<Process> p = get_process_by_id(pid);

    if (!p || p->state == ProcessState::TERMINATED) {
        std::cout << "[ERROR] Proceso ID " << pid << " no encontrado o ya terminado." << std::endl;
        return false;
    }

    // Delega la remoción de la cola (READY/RUNNING/BLOCKED) al Scheduler
    // Nota: El scheduler se encargará de reportar si el proceso fue matado.
    if (scheduler->kill_process(pid)) {
        // En este punto, el proceso está marcado como TERMINATED en el scheduler.
        return true;
    }

    // Este caso solo debería ocurrir si el scheduler no lo encuentra (lo cual ya maneja el scheduler)
    return false;
}

// IMPLEMENTACIÓN DE COMANDOS DE MEMORIA


// ⭐️ FUNCION ANTERIOR RENOMBRADA: Usada ahora por la CLI
void KernelSimulator::access_memory_cli(int virtual_address) {
    // 1. Obtener el proceso que está RUNNING
    int pid = scheduler->get_running_process_id();

    if (pid == -1) {
        std::cout << "[MEM] Error: No hay proceso en RUNNING para acceder a memoria." << std::endl;
        return;
    }

    // El PID se extrae de 'p' y se pasa a la función principal de acceso.
    this->access_memory(pid, virtual_address);
}

//  FUNCION PRINCIPAL DE ACCESO (Ahora toma PID y Address)
void KernelSimulator::access_memory(int pid, int virtual_address) {
    // Obtener el proceso por ID para acceder a su tabla de páginas
    std::shared_ptr<Process> p = get_process_by_id(pid);

    if (!p) {
        std::cout << "[MEM] Error: Proceso ID " << pid << " no encontrado para acceso a memoria." << std::endl;
        return;
    }

    // Ejecutar la función de acceso del gestor de memoria
    memory_manager->access_memory(p->id, virtual_address, p->page_table, current_time);
}

void KernelSimulator::print_memory_status() {
    memory_manager->print_memory_status();
}

// Funciones privadas
void KernelSimulator::check_for_interrupts() {
    // Lógica para revisar si hay interrupciones (sin implementar)
}

// ⭐ NUEVO: Helper para obtener proceso por ID (necesario para el desbloqueo y acceso)
std::shared_ptr<Process> KernelSimulator::get_process_by_id(int pid) {
    auto it = std::find_if(all_processes.begin(), all_processes.end(),
        [pid](const std::shared_ptr<Process>& p) { return p->id == pid; });

    if (it != all_processes.end()) {
        return *it;
    }
    return nullptr;
}


//  IMPLEMENTACIÓN DE SINCRONIZACIÓN Y HELPERS 

// FUNCION PARA LA CLI (get_running_process_id)
int KernelSimulator::get_running_process_id() const {
    return scheduler->get_running_process_id();
}

void KernelSimulator::produce_resource(const std::string& item) {
    int producer_pid = get_running_process_id();
    // Obtener el shared_ptr del proceso RUNNING para pasarlo al scheduler para bloqueo
    std::shared_ptr<Process> p = get_process_by_id(producer_pid);

    if (producer_pid == -1) {
        std::cout << "[ERROR SYNC] No hay un proceso en ejecución para producir." << std::endl;
        return;
    }

    std::cout << "[PRODUCER " << producer_pid << "] Intenta producir recurso: " << item << std::endl;

    // 1. wait(empty): Esperar a que haya espacio vacío
    if (!empty.wait(producer_pid)) {
        std::cout << "  > [WAIT-EMPTY] Buffer lleno. Proceso " << producer_pid << " pasa a BLOQUEADO." << std::endl;
        // Mover el proceso de RUNNING a la cola BLOCKED del Scheduler
        if (p) scheduler->block_running_process(p);
        return;
    }

    // 2. wait(mutex): Esperar a obtener acceso exclusivo
    if (!mutex.wait(producer_pid)) {
        std::cout << "  > [WAIT-MUTEX] Mutex bloqueado. Proceso " << producer_pid << " pasa a BLOQUEADO." << std::endl;
        // Mover el proceso de RUNNING a la cola BLOCKED del Scheduler
        if (p) scheduler->block_running_process(p);
        return;
    }


    pc_buffer.push_back(item);
    std::cout << "  > [PRODUCED] Item '" << item << "' añadido. Buffer: " << pc_buffer.size() << "/" << PC_BUFFER_SIZE << std::endl;


    // 3. signal(mutex): Liberar el mutex
    int unblocked_pid_m = mutex.signal();
    if (unblocked_pid_m != -1) {
        std::shared_ptr<Process> unblocked_p = get_process_by_id(unblocked_pid_m);
        if (unblocked_p) {
            scheduler->unblock_process(unblocked_p);
            std::cout << "  > [SIGNAL-MUTEX] Desbloquea a PID " << unblocked_pid_m << " (lo pone en READY)." << std::endl;
        }
    }

    // 4. signal(full): Indicar que hay un nuevo recurso lleno
    int unblocked_pid_f = full.signal();
    if (unblocked_pid_f != -1) {
        std::shared_ptr<Process> unblocked_p = get_process_by_id(unblocked_pid_f);
        if (unblocked_p) {
            scheduler->unblock_process(unblocked_p);
            std::cout << "  > [SIGNAL-FULL] Desbloquea a PID " << unblocked_pid_f << " (lo pone en READY)." << std::endl;
        }
    }
}

void KernelSimulator::consume_resource() {
    int consumer_pid = get_running_process_id();
    std::shared_ptr<Process> p = get_process_by_id(consumer_pid);

    if (consumer_pid == -1) {
        std::cout << "[ERROR SYNC] No hay un proceso en ejecución para consumir." << std::endl;
        return;
    }

    std::cout << "[CONSUMER " << consumer_pid << "] Intenta consumir recurso." << std::endl;

    // 1. wait(full): Esperar a que haya recursos llenos
    if (!full.wait(consumer_pid)) {
        std::cout << "  > [WAIT-FULL] Buffer vacío. Proceso " << consumer_pid << " pasa a BLOQUEADO." << std::endl;
        if (p) scheduler->block_running_process(p);
        return;
    }

    // 2. wait(mutex): Esperar a obtener acceso exclusivo
    if (!mutex.wait(consumer_pid)) {
        std::cout << "  > [WAIT-MUTEX] Mutex bloqueado. Proceso " << consumer_pid << " pasa a BLOQUEADO." << std::endl;
        if (p) scheduler->block_running_process(p);
        return;
    }

    
    std::string consumed_item = "N/A";
    if (!pc_buffer.empty()) {
        consumed_item = pc_buffer.front();
        pc_buffer.pop_front();
    }
    std::cout << "  > [CONSUMED] Item '" << consumed_item << "' consumido. Buffer: " << pc_buffer.size() << "/" << PC_BUFFER_SIZE << std::endl;
    

    // 3. signal(mutex): Liberar el mutex
    int unblocked_pid_m = mutex.signal();
    if (unblocked_pid_m != -1) {
        std::shared_ptr<Process> unblocked_p = get_process_by_id(unblocked_pid_m);
        if (unblocked_p) {
            scheduler->unblock_process(unblocked_p);
            std::cout << "  > [SIGNAL-MUTEX] Desbloquea a PID " << unblocked_pid_m << " (lo pone en READY)." << std::endl;
        }
    }

    // 4. signal(empty): Indicar que hay un nuevo espacio vacío
    int unblocked_pid_e = empty.signal();
    if (unblocked_pid_e != -1) {
        std::shared_ptr<Process> unblocked_p = get_process_by_id(unblocked_pid_e);
        if (unblocked_p) {
            scheduler->unblock_process(unblocked_p);
            std::cout << "  > [SIGNAL-EMPTY] Desbloquea a PID " << unblocked_pid_e << " (lo pone en READY)." << std::endl;
        }
    }
}

void KernelSimulator::print_pc_status() const {
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Estado del Buffer Productor-Consumidor:" << std::endl;
    std::cout << "Tamaño del Buffer: " << pc_buffer.size() << " / " << PC_BUFFER_SIZE << std::endl;

    std::cout << "Contenido: [ ";
    for (const auto& item : pc_buffer) {
        std::cout << item << " | ";
    }
    std::cout << "]" << std::endl;

    std::cout << "---------------------------------------" << std::endl;
    mutex.print_status("Mutex");
    empty.print_status("Empty");
    full.print_status("Full");
    std::cout << "---------------------------------------" << std::endl;
}