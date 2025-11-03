// cli/commands.cpp

#include "commands.h"
#include "kernel/core.h"
#include "kernel/scheduler.h"
#include "../modules/mem/memory_manager.h"
#include "../modules/disk/disk_scheduler.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <vector>

extern KernelSimulator kernel;

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
			else if (algo == "pff") {
				kernel.set_replacement_algorithm(ReplacementAlgorithm::PFF);
				std::cout << "[INFO] Algoritmo de reemplazo de páginas cambiado a: PFF (Page Fault Frequency)." << std::endl;
			}
			else {
				std::cout << "[ERROR] Algoritmo no reconocido. Opciones: fifo, lru, pff." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] 'set_page_algo' requiere especificar el algoritmo (ej. set_page_algo lru)." << std::endl;
		}
	}
	else if (command == "access") {
		int address;
		int target_pid;

		if (command_parts.size() == 2) {
			if (std::stringstream(command_parts[1]) >> address && address >= 0) {
				target_pid = kernel.get_running_process_id();
			}
			else {
				std::cout << "[ERROR] 'access' requiere una dirección virtual positiva (ej. access 2000)." << std::endl;
				return;
			}
		}
		else if (command_parts.size() == 3) {
			if (std::stringstream(command_parts[1]) >> address && address >= 0 &&
				std::stringstream(command_parts[2]) >> target_pid && target_pid > 0) {
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

		kernel.access_memory(target_pid, address);
	}
	else if (command == "disk_req" || command == "dreq") {
		int cylinder;
		int target_pid = -1;

		if (command_parts.size() == 2) {
			if (std::stringstream(command_parts[1]) >> cylinder && cylinder >= 0) {
				kernel.disk_request(cylinder);
			}
			else {
				std::cout << "[ERROR] 'dreq' requiere un número de cilindro válido (0-199)." << std::endl;
			}
		}
		else if (command_parts.size() == 3) {
			if (std::stringstream(command_parts[1]) >> cylinder && cylinder >= 0 &&
				std::stringstream(command_parts[2]) >> target_pid && target_pid > 0) {
				kernel.disk_request(target_pid, cylinder);
			}
			else {
				std::cout << "[ERROR] Formato incorrecto. Uso: 'dreq <cilindro>' o 'dreq <cilindro> <pid>'." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] 'dreq' requiere al menos el número de cilindro." << std::endl;
		}
	}
	else if (command == "disk_process" || command == "dproc") {
		kernel.process_disk_request();
	}
	else if (command == "disk_status" || command == "dstat") {
		kernel.print_disk_status();
	}
	else if (command == "disk_stats") {
		kernel.print_disk_stats();
	}
	else if (command == "disk_view" || command == "dview") {
		kernel.visualize_disk();
	}
	else if (command == "set_disk_algo") {
		if (command_parts.size() > 1) {
			std::string algo = command_parts[1];
			std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

			if (algo == "fcfs") {
				kernel.set_disk_algorithm(DiskAlgorithm::FCFS);
				std::cout << "[INFO] Algoritmo de disco cambiado a: FCFS." << std::endl;
			}
			else if (algo == "sstf") {
				kernel.set_disk_algorithm(DiskAlgorithm::SSTF);
				std::cout << "[INFO] Algoritmo de disco cambiado a: SSTF." << std::endl;
			}
			else if (algo == "scan") {
				kernel.set_disk_algorithm(DiskAlgorithm::SCAN);
				std::cout << "[INFO] Algoritmo de disco cambiado a: SCAN." << std::endl;
			}
			else {
				std::cout << "[ERROR] Algoritmo no reconocido. Opciones: fcfs, sstf, scan." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] 'set_disk_algo' requiere especificar el algoritmo (ej. set_disk_algo sstf)." << std::endl;
		}
	}
	else if (command == "ioreq") {
		if (command_parts.size() < 3) {
			std::cout << "[ERROR] Uso: ioreq <prioridad> <tiempo> [datos]" << std::endl;
			return;
		}

		int priority, service_time;
		if (!(std::stringstream(command_parts[1]) >> priority && priority >= 0) ||
			!(std::stringstream(command_parts[2]) >> service_time && service_time > 0)) {
			std::cout << "[ERROR] Prioridad y tiempo deben ser números válidos." << std::endl;
			return;
		}

		std::string data = (command_parts.size() > 3) ? command_parts[3] : "default";
		kernel.io_request(priority, service_time, data);
	}
	else if (command == "ioproc") {
		kernel.process_io();
	}
	else if (command == "iostat") {
		kernel.print_io_status();
	}
	else if (command == "io_stats") {
		kernel.print_io_stats();
	}
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
	else if (command == "phil") {
		if (command_parts.size() < 2) {
			std::cout << "[ERROR] Uso: phil <start|stop|status|think|eat> [id]" << std::endl;
			return;
		}

		std::string subcmd = command_parts[1];
		std::transform(subcmd.begin(), subcmd.end(), subcmd.begin(), ::tolower);

		if (subcmd == "start") {
			kernel.start_philosophers();
		}
		else if (subcmd == "stop") {
			kernel.stop_philosophers();
		}
		else if (subcmd == "status") {
			kernel.print_philosophers_status();
		}
		else if (subcmd == "think" && command_parts.size() == 3) {
			int id;
			if (std::stringstream(command_parts[2]) >> id && id >= 0 && id < 5) {
				kernel.philosopher_think(id);
			}
			else {
				std::cout << "[ERROR] ID de filósofo inválido (0-4)." << std::endl;
			}
		}
		else if (subcmd == "eat" && command_parts.size() == 3) {
			int id;
			if (std::stringstream(command_parts[2]) >> id && id >= 0 && id < 5) {
				kernel.philosopher_eat(id);
			}
			else {
				std::cout << "[ERROR] ID de filósofo inválido (0-4)." << std::endl;
			}
		}
		else {
			std::cout << "[ERROR] Subcomando desconocido. Uso: phil <start|stop|status|think|eat> [id]" << std::endl;
		}
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
		std::cout << "\n[PROCESOS]" << std::endl;
		std::cout << "new <burst>: Crea un proceso con el tiempo de ráfaga especificado." << std::endl;
		std::cout << "ps: Lista todos los procesos y su estado." << std::endl;
		std::cout << "kill <id>: Termina el proceso con el ID especificado." << std::endl;

		std::cout << "\n[PLANIFICACIÓN CPU]" << std::endl;
		std::cout << "run <n>: Ejecuta 'n' ciclos de reloj." << std::endl;
		std::cout << "set_algo <rr|sjf>: Selecciona algoritmo de planificación." << std::endl;
		std::cout << "set_quantum <n>: Establece el Quantum para Round Robin." << std::endl;

		std::cout << "\n[MEMORIA]" << std::endl;
		std::cout << "set_page_algo <fifo|lru|pff>: Algoritmo de reemplazo de páginas." << std::endl;
		std::cout << "access <dir> [pid]: Simula acceso a memoria virtual." << std::endl;
		std::cout << "memview: Muestra estado de la memoria física." << std::endl;

		std::cout << "\n[DISCO]" << std::endl;
		std::cout << "dreq <cilindro> [pid]: Solicitud de acceso a disco." << std::endl;
		std::cout << "dproc: Procesa siguiente solicitud de disco." << std::endl;
		std::cout << "dstat: Muestra estado del planificador de disco." << std::endl;
		std::cout << "disk_stats: Muestra estadísticas de disco." << std::endl;
		std::cout << "dview: Visualiza posición del cabezal y solicitudes." << std::endl;
		std::cout << "set_disk_algo <fcfs|sstf|scan>: Cambia algoritmo de disco." << std::endl;

		std::cout << "\n[E/S]" << std::endl;
		std::cout << "ioreq <prioridad> <tiempo> [datos]: Solicitud de E/S." << std::endl;
		std::cout << "ioproc: Procesa operación de E/S." << std::endl;
		std::cout << "iostat: Estado del sistema de E/S." << std::endl;
		std::cout << "io_stats: Estadísticas de E/S." << std::endl;

		std::cout << "\n[SINCRONIZACIÓN]" << std::endl;
		std::cout << "produce [item]: El proceso RUNNING produce un recurso." << std::endl;
		std::cout << "consume: El proceso RUNNING consume un recurso." << std::endl;
		std::cout << "sync_stat: Muestra estado del buffer P/C." << std::endl;

		std::cout << "\n[FILÓSOFOS]" << std::endl;
		std::cout << "phil start: Inicia simulación de filósofos." << std::endl;
		std::cout << "phil stop: Detiene simulación." << std::endl;
		std::cout << "phil status: Estado de los filósofos." << std::endl;
		std::cout << "phil think <id>: Filósofo ID piensa (0-4)." << std::endl;
		std::cout << "phil eat <id>: Filósofo ID intenta comer (0-4)." << std::endl;

		std::cout << "\n[GENERAL]" << std::endl;
		std::cout << "stats: Muestra todas las métricas de rendimiento." << std::endl;
		std::cout << "exit: Sale del simulador." << std::endl;
		std::cout << "----------------------------\n" << std::endl;
		}

	else {
		std::cout << "[ERROR] Comando desconocido. Use 'help'." << std::endl;
	}
}