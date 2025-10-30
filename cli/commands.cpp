// cli/commands.cpp

#include "commands.h"
#include "kernel/core.h" // Necesario para acceder a KernelSimulator
#include "kernel/scheduler.h" // Necesario para acceder a SchedulerAlgorithm
#include "../modules/mem/memory_manager.h" // Necesario para acceder a ReplacementAlgorithm
#include <iostream>
#include <sstream>
#include <cstdlib> // Para exit()
#include <algorithm>
#include <cctype>
#include <vector>

// IMPORTANTE: La instancia 'kernel' debe ser declarada como 'extern'
// para que el linker la encuentre, ya que está definida en main.cpp.
extern KernelSimulator kernel;

// Función auxiliar para parsear la línea de comando
std::vector<std::string> parse_command_line(const std::string& line) {
	std::vector<std::string> parts;
	std::stringstream ss(line);
	std::string part;
	while (ss >> part) {
		parts.push_back(part);
	}
	return parts;
}

void handle_command(const std::string& line) {
	if (line.empty()) {
		return;
	}

	std::vector<std::string> command_parts = parse_command_line(line);
	if (command_parts.empty()) return;

	std::string command = command_parts[0];
	
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);

	// --- LÓGICA DE COMANDOS SIMPLES ---
	if (command == "new") {
		int burst;
		if (command_parts.size() > 1 && std::stringstream(command_parts[1]) >> burst && burst > 0) {
			kernel.new_process(burst);
		}
		else {
			std::cout << "[ERROR] 'new' requiere un tiempo de ráfaga (burst > 0)." << std::endl;
		}
	}
	else if (command == "tick") {
		kernel.run_tick();
	}
	else if (command == "run") {
		int cycles;
		if (command_parts.size() > 1 && std::stringstream(command_parts[1]) >> cycles && cycles > 0) {
			kernel.run_cycles(cycles);
		}
		else {
			std::cout << "[ERROR] 'run' requiere un número de ciclos positivo." << std::endl;
		}
	}
	else if (command == "ps") {
		kernel.list_processes();
	}
	else if (command == "stats") {
		kernel.print_stats();
	}
	else if (command == "memview") {
		kernel.print_memory_status();
	}
	
	// ⭐️ COMANDOS 'set' SIMPLIFICADOS (set_algo, set_quantum, set_page_algo) ⭐️
	
	else if (command == "set_quantum") {
		if (command_parts.size() == 2) {
			int quantum;
			if (std::stringstream(command_parts[1]) >> quantum && quantum > 0) {
				kernel.set_quantum(quantum);
			}
			else {
				std::cout << "[ERROR] 'set_quantum' requiere un valor de quantum positivo." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] Uso: set_quantum <n>" << std::endl;
		}
	}
	else if (command == "set_algo") {
		if (command_parts.size() > 1) {
			std::string algo = command_parts[1];
			std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

			if (algo == "rr") {
				kernel.set_scheduler_algorithm(SchedulerAlgorithm::ROUND_ROBIN);
				std::cout << "[INFO] Planificador cambiado a: Round Robin (RR)." << std::endl;
			}
			else if (algo == "sjf") {
				kernel.set_scheduler_algorithm(SchedulerAlgorithm::SJF);
				std::cout << "[INFO] Planificador cambiado a: Shortest Job First (SJF) No Expropiativo." << std::endl;
			}
			else {
				std::cout << "[ERROR] Algoritmo no reconocido. Opciones: rr, sjf." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] 'set_algo' requiere especificar el algoritmo (ej. set_algo sjf)." << std::endl;
		}
	}
	// ⭐️ COMANDO DE REEMPLAZO DE PÁGINAS (FIFO / LRU) ⭐️
	else if (command == "set_page_algo") {
		if (command_parts.size() > 1) {
			std::string algo = command_parts[1];
			std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

			if (algo == "fifo") {
				kernel.set_replacement_algorithm(ReplacementAlgorithm::FIFO);
				std::cout << "[INFO] Algoritmo de reemplazo de páginas cambiado a: FIFO." << std::endl;
			}
			else if (algo == "lru") {
				kernel.set_replacement_algorithm(ReplacementAlgorithm::LRU);
				std::cout << "[INFO] Algoritmo de reemplazo de páginas cambiado a: LRU (Least Recently Used)." << std::endl;
			}
			else {
				std::cout << "[ERROR] Algoritmo no reconocido. Opciones: fifo, lru." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] 'set_page_algo' requiere especificar el algoritmo (ej. set_page_algo lru)." << std::endl;
		}
	}
	
	// ⭐️ LÓGICA DE ACCESO (mantenida y separada) ⭐️
	
	else if (command == "access") {
		int address;
		int target_pid;

		if (command_parts.size() == 2) {
			// Caso 1: access <address> (Usa PID RUNNING)
			if (std::stringstream(command_parts[1]) >> address && address >= 0) {
				target_pid = kernel.get_running_process_id();
			}
			else {
				std::cout << "[ERROR] 'access' requiere una dirección virtual positiva (ej. access 2000)." << std::endl;
				return;
			}
		}
		else if (command_parts.size() == 3) {
			// Caso 2: access <address> <pid> (Usa PID especificado)
			if (std::stringstream(command_parts[1]) >> address && address >= 0 &&
				std::stringstream(command_parts[2]) >> target_pid && target_pid > 0) {
				// OK: Usar target_pid
			}
			else {
				std::cout << "[ERROR] Formato incorrecto. Uso: 'access <dir>' o 'access <dir> <pid>'." << std::endl;
				return;
			}
		}
		else {
			std::cout << "[ERROR] 'access' requiere una dirección (y opcionalmente un PID)." << std::endl;
			return;
		}

		if (target_pid == -1) {
			std::cout << "[ERROR] No hay un proceso en ejecución para acceder a la memoria (PID no encontrado)." << std::endl;
			return;
		}

		// LLAMAR CON EL PID Y LA DIRECCIÓN CORRECTOS
		kernel.access_memory(target_pid, address);
	}
	
	// ⭐️ COMANDOS DE SINCRONIZACIÓN ⭐️
	
	else if (command == "produce") {
		std::string item_name = (command_parts.size() > 1) ? command_parts[1] : "";
		kernel.produce_resource(item_name);
	}
	else if (command == "consume") {
		kernel.consume_resource();
	}
	else if (command == "sync_stat") {
		kernel.print_pc_status();
	}
	
	else if (command == "kill") {
		int id;
		if (command_parts.size() > 1 && std::stringstream(command_parts[1]) >> id) {
			kernel.kill_process(id);
		}
		else {
			std::cout << "[ERROR] 'kill' requiere el ID del proceso." << std::endl;
		}
	}
	else if (command == "exit") {
		std::cout << "Saliendo del simulador." << std::endl;
		exit(0);
	}
	else if (command == "help") {
		
		std::cout << "\n--- Comandos Disponibles ---" << std::endl;
		std::cout << "new <burst>: Crea un proceso con el tiempo de ráfaga (CPU) especificado." << std::endl;
		std::cout << "ps: Lista todos los procesos con su estado actual y estadísticas." << std::endl;
		std::cout << "run <n>: Ejecuta 'n' ciclos de reloj." << std::endl;
		std::cout << "set_algo <rr|sjf>: Selecciona el algoritmo de planificación de CPU (RR o SJF)." << std::endl;
		std::cout << "set_quantum <n>: Establece el valor del Quantum para Round Robin." << std::endl; // 🆕 LÍNEA ACTUALIZADA
		std::cout << "set_page_algo <fifo|lru>: Selecciona el algoritmo de reemplazo de páginas (FIFO o LRU)." << std::endl;
		std::cout << "access <dir> [pid]: Simula acceso a una dirección virtual para el PID especificado (opcional)." << std::endl;
		std::cout << "memview: Muestra el estado actual de la memoria física." << std::endl;
		std::cout << "stats: Muestra métricas de rendimiento (Scheduler y Memoria)." << std::endl;
		std::cout << "produce [item]: El proceso RUNNING produce un recurso." << std::endl;
		std::cout << "consume: El proceso RUNNING consume un recurso." << std::endl;
		std::cout << "sync_stat: Muestra el estado del buffer P/C y los semáforos." << std::endl;
		std::cout << "kill <id>: Termina el proceso con el ID especificado." << std::endl;
		std::cout << "exit: Sale del simulador." << std::endl;
		std::cout << "----------------------------" << std::endl;
	}
	else {
		std::cout << "[ERROR] Comando desconocido. Use 'help'." << std::endl;
	}
}