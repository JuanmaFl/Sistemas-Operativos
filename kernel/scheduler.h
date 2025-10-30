#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include "process.h"
#include <deque>
#include <memory> // Para std::shared_ptr
#include <algorithm> // Necesario para std::find en la implementación

enum class SchedulerAlgorithm {
	ROUND_ROBIN,
	SJF
	// Se pueden añadir más algoritmos aquí
};

class Scheduler {
private:
	std::deque<std::shared_ptr<Process>> ready_queue;
	std::shared_ptr<Process> running_process = nullptr;
	// ⭐️ NUEVO: Cola de procesos bloqueados/espera ⭐️
	std::deque<std::shared_ptr<Process>> blocked_queue;


	int quantum_size;
	SchedulerAlgorithm algorithm;

public:
	// Constructor: Usamos quantum_size en lugar de time_quantum
	Scheduler(int quantum = 2, SchedulerAlgorithm algo = SchedulerAlgorithm::ROUND_ROBIN);
	void add_process(std::shared_ptr<Process> p);
	void tick(int current_time);

	std::shared_ptr<Process> get_running_process() const { return running_process; }

	// ⭐️ CORRECCIÓN CLAVE 1: Declaración de la función usada por el Kernel ⭐️
	int get_running_process_id() const;

	// Función central: decide qué proceso pasa a RUNNING o regresa a READY
	std::shared_ptr<Process> dispatch();

	// Setter para el algoritmo (útil para la CLI)
	void set_algorithm(SchedulerAlgorithm algo);

	//  NUEVOS MÉTODOS PARA SINCRONIZACIÓN (Módulo 3) 
	void block_running_process(std::shared_ptr<Process> p);
	void unblock_process(std::shared_ptr<Process> p);
	bool kill_process(int pid);

	// Setter para el Quantum (Declaración)
	void set_quantum(int new_quantum);

	// Opcionalmente, un getter (CORREGIDO: usa quantum_size)
	int get_quantum() const { return quantum_size; }
};

#endif 