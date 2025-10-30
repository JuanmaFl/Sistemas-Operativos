#ifndef KERNEL_CORE_H
#define KERNEL_CORE_H

// LIBRERÍAS C++ Y PUNTEROS INTELIGENTES
#include <memory>
#include <vector>
#include <deque> // Necesario para el buffer P/C
#include <string> // Necesario para el buffer P/C

// 1. INCLUSIONES CLAVE DEL MÓDULO (MemoryManager debe ser visible)
#include "../modules/mem/memory_manager.h"

// 2. INCLUSIONES DEL KERNEL
#include "process.h"
#include "scheduler.h"
#include "sync.h" // ⭐️ NUEVO: Módulo de Semáforos

// Definición del tamaño del buffer Productor-Consumidor
const int PC_BUFFER_SIZE = 5;

class KernelSimulator {
private:
	std::unique_ptr<Scheduler> scheduler; // El planificador
	std::unique_ptr<MemoryManager> memory_manager; // GESTOR DE MEMORIA
	int current_time; // Tiempo global del simulador (ticks)
	std::vector<std::shared_ptr<Process>> all_processes; // Lista de todos los procesos creados

	// MÓDULO DE SINCRONIZACIÓN (Productor-Consumidor)
	std::deque<std::string> pc_buffer; // El buffer de recursos (simulado)

	// Semáforos para el problema Productor-Consumidor
	Semaphore mutex = Semaphore(1); // Exclusión mutua (semaforo binario)
	Semaphore empty = Semaphore(PC_BUFFER_SIZE); // Espacios vacíos
	Semaphore full = Semaphore(0); // Espacios llenos (ítems producidos)

public:
	KernelSimulator();
	~KernelSimulator();
	void print_stats();

	// Lógica principal de la simulación
	void run_tick();
	void run_cycles(int n);

	// Comandos de la CLI
	void new_process(int burst);
	void list_processes();

	// ⭐️ NUEVO: Expone el comando kill a la CLI
	bool kill_process(int pid);

	// ⭐️ Setter para el algoritmo del Planificador ⭐️
	void set_scheduler_algorithm(SchedulerAlgorithm algo) {
		if (scheduler) {
			scheduler->set_algorithm(algo);
		}
	}

	//  NUEVO: Setter para el Quantum 
	//  CORRECCIÓN CLAVE 2: Solo la declaración (la definición va en core.cpp) 
	void set_quantum(int quantum);

	//  Setter para el algoritmo de reemplazo de páginas 
	void set_replacement_algorithm(ReplacementAlgorithm algo) {
		if (memory_manager) {
			memory_manager->set_replacement_algorithm(algo);
		}
	}

	// Funciones de Memoria (DECLARACIONES)
	// Función principal (usada internamente/por CLI)
	void access_memory(int pid, int virtual_address);
	// Función auxiliar para la CLI (llama a access_memory con el PID RUNNING)
	void access_memory_cli(int virtual_address);
	void print_memory_status();
	void print_memory_stats();

	// ⭐️ CORRECCIÓN: Agregamos 'const' ⭐️
	int get_running_process_id() const;

	// ⭐️ FUNCIONES DE SINCRONIZACIÓN (DECLARACIONES) ⭐️
	void produce_resource(const std::string& item);
	void consume_resource();
	void print_pc_status() const; // Comando 'sync_stat'

private:
	// Funciones internas del núcleo (no expuestas a la CLI)
	void check_for_interrupts();

	// Función auxiliar para obtener proceso por ID
	std::shared_ptr<Process> get_process_by_id(int pid);
};

#endif // KERNEL_CORE_H