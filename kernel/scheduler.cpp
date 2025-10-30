// kernel/scheduler.cpp

#include "scheduler.h"
#include <iostream>
#include <algorithm> // Necesario para std::find, std::find_if, e iteradores
#include <memory>


Scheduler::Scheduler(int quantum, SchedulerAlgorithm algo)
	: quantum_size(quantum), algorithm(algo) {
	
	std::cout << "[SCHEDULER] Inicializado con algoritmo " << (algo == SchedulerAlgorithm::SJF ? "SJF" : "RR") << " (Quantum: " << quantum_size << ")." << std::endl;
}

void Scheduler::set_algorithm(SchedulerAlgorithm algo) {
	algorithm = algo;
}

void Scheduler::add_process(std::shared_ptr<Process> p) {
	// Solo NEW pasa a READY.
	if (p->state == ProcessState::NEW) {
		p->state = ProcessState::READY;
		p->reset_quantum_counter();
		ready_queue.push_back(p);
	}
}


std::shared_ptr<Process> Scheduler::dispatch() {
	if (ready_queue.empty()) {
		running_process = nullptr;
		return nullptr;
	}

	if (algorithm == SchedulerAlgorithm::ROUND_ROBIN) {
		// --- Lógica Round Robin ---
		running_process = ready_queue.front();
		ready_queue.pop_front();
	}
	else if (algorithm == SchedulerAlgorithm::SJF) {
		// --- Lógica Shortest Job First (SJF) No Expropiativo ---

		// 1. Encontrar el proceso con el menor tiempo restante (remaining_time)
		auto shortest_it = ready_queue.begin();
		long min_time = (*shortest_it)->remaining_time;

		auto current_it = ready_queue.begin();
		while (++current_it != ready_queue.end()) {
			if ((*current_it)->remaining_time < min_time) {
				min_time = (*current_it)->remaining_time;
				shortest_it = current_it;
			}
		}

		// 2. Tomar el proceso más corto y removerlo de la cola
		running_process = *shortest_it;
		ready_queue.erase(shortest_it);
	}

	running_process->state = ProcessState::RUNNING;
	running_process->reset_quantum_counter();

	return running_process;
}

void Scheduler::tick(int current_time) {
	// 1. Manejar procesos en cola (esperando)
	for (auto& p : ready_queue) {
		p->wait_time++;
	}

	// 2. Ejecutar el proceso RUNNING (si existe)
	if (running_process) {
		running_process->run_tick();

		// 3. Evaluar el estado después de la ejecución

		// A) ¿El proceso TERMINÓ?
		if (running_process->remaining_time == 0) {
			running_process->state = ProcessState::TERMINATED;

			running_process->turnaround_time = current_time - running_process->arrival_time;

			std::cout << "[SCHEDULER] Proceso " << running_process->id
				<< " TERMINADO. (Turnaround: " << running_process->turnaround_time
				<< ", Espera: " << running_process->wait_time << ")" << std::endl;
			running_process = nullptr;
		}

	
		else if (algorithm == SchedulerAlgorithm::ROUND_ROBIN && running_process->time_on_cpu >= quantum_size) {
			std::cout << "[SCHEDULER] Proceso " << running_process->id << " EXPROPIADO (Quantum agotado)." << std::endl;

			running_process->state = ProcessState::READY;
			ready_queue.push_back(running_process);
			running_process = nullptr;

			dispatch();
			return;
		}

		
	}

	// 4. Despachar el siguiente proceso si el CPU está libre
	if (!running_process) {
		dispatch();
	}
}


// MÉTODOS AUXILIARES Y SINCRONIZACIÓN


/**
 * @brief Devuelve el ID del proceso actualmente en RUNNING.
 */
int Scheduler::get_running_process_id() const {
	if (running_process) {
		return running_process->id;
	}
	return -1;
}

/**
 * @brief Mueve el proceso actualmente RUNNING a la cola BLOCKED (WAITING).
 */
void Scheduler::block_running_process(std::shared_ptr<Process> p) {
	if (running_process && running_process->id == p->id) {
		p->state = ProcessState::WAITING;
		blocked_queue.push_back(p);

		std::cout << "[SCHEDULER] Bloqueo: PID " << p->id << " movido a WAITING (recurso). Context Switch necesario." << std::endl;

		running_process = nullptr;
		dispatch();
	}
}

/**
 * @brief Mueve un proceso de la cola BLOCKED a la cola READY (despertar).
 */
void Scheduler::unblock_process(std::shared_ptr<Process> p) {
	auto it = std::find_if(blocked_queue.begin(), blocked_queue.end(),
		[p](const std::shared_ptr<Process>& blocked_p) {
			return blocked_p->id == p->id;
		});

	if (it != blocked_queue.end()) {
		std::shared_ptr<Process> target_p = *it;
		blocked_queue.erase(it);

		target_p->state = ProcessState::READY;
		ready_queue.push_back(target_p);

		std::cout << "[SCHEDULER] Desbloqueo: PID " << target_p->id << " movido a READY (recurso disponible)." << std::endl;
	}
}

//  IMPLEMENTACIÓN DEL COMANDO KILL (Corregido) 

/**
 * @brief Termina el proceso con el ID especificado, sin importar su estado.
 * @return True si se encontró y mató el proceso, false en caso contrario.
 */
bool Scheduler::kill_process(int pid) {
	std::shared_ptr<Process> target_p = nullptr;
	bool was_running = false;

	// 1. Buscar en RUNNING
	if (running_process && running_process->id == pid) {
		target_p = running_process;
		was_running = true;
		running_process = nullptr;	// Libera la CPU
	}
	// 2. Buscar en READY
	else {
		auto ready_it = std::find_if(ready_queue.begin(), ready_queue.end(),
			[pid](const std::shared_ptr<Process>& p) { return p->id == pid; });

		if (ready_it != ready_queue.end()) {
			target_p = *ready_it;
			ready_queue.erase(ready_it);
		}
	}
	// 3. Buscar en BLOCKED (WAITING)
	if (!target_p) {
		auto blocked_it = std::find_if(blocked_queue.begin(), blocked_queue.end(),
			[pid](const std::shared_ptr<Process>& p) { return p->id == pid; });

		if (blocked_it != blocked_queue.end()) {
			target_p = *blocked_it;
			blocked_queue.erase(blocked_it);
			// Nota: Aquí se podría añadir lógica para liberar recursos bloqueados por este proceso.
		}
	}

	if (target_p) {
		// 4. Marcar como TERMINATED y reportar
		target_p->state = ProcessState::TERMINATED;
		std::cout << "[SCHEDULER] Proceso " << pid << " terminado forzosamente (KILL)." << std::endl;

		// 5. Despachar si se liberó el CPU
		if (was_running) {
			dispatch();
		}

		return true;
	}
	else {
		std::cout << "[ERROR] Proceso " << pid << " no encontrado en READY, RUNNING, o WAITING/BLOCKED." << std::endl;
		return false;
	}
}

// Implementación de set_quantum
void Scheduler::set_quantum(int new_quantum) {
	if (new_quantum > 0) {
		
		quantum_size = new_quantum;
		std::cout << "[INFO] Quantum cambiado a: " << new_quantum << "." << std::endl;
	}
	else {
		std::cerr << "[ERROR] El Quantum debe ser un valor positivo." << std::endl;
	}
}