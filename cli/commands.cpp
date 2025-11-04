// cli/commands.cpp

#include "commands.h"
#include "colors.h"
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
#include <fstream>

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
			print_error("'new' requiere un tiempo de ráfaga (burst > 0).");
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
			print_error("'run' requiere un número de ciclos positivo.");
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
				print_error("'set_quantum' requiere un valor de quantum positivo.");
			}
		}
		else {
			print_error("Uso: set_quantum <n>");
		}
	}
	else if (command == "set_algo") {
		if (command_parts.size() > 1) {
			std::string algo = command_parts[1];
			std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

			if (algo == "rr") {
				kernel.set_scheduler_algorithm(SchedulerAlgorithm::ROUND_ROBIN);
				print_info("Planificador cambiado a: Round Robin (RR).");
			}
			else if (algo == "sjf") {
				kernel.set_scheduler_algorithm(SchedulerAlgorithm::SJF);
				print_info("Planificador cambiado a: Shortest Job First (SJF) No Expropiativo.");
			}
			else {
				print_error("Algoritmo no reconocido. Opciones: rr, sjf.");
			}
		}
		else {
			print_error("'set_algo' requiere especificar el algoritmo (ej. set_algo sjf).");
		}
	}
	else if (command == "set_page_algo") {
		if (command_parts.size() > 1) {
			std::string algo = command_parts[1];
			std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

			if (algo == "fifo") {
				kernel.set_replacement_algorithm(ReplacementAlgorithm::FIFO);
				print_info("Algoritmo de reemplazo de páginas cambiado a: FIFO.");
			}
			else if (algo == "lru") {
				kernel.set_replacement_algorithm(ReplacementAlgorithm::LRU);
				print_info("Algoritmo de reemplazo de páginas cambiado a: LRU (Least Recently Used).");
			}
			else if (algo == "pff") {
				kernel.set_replacement_algorithm(ReplacementAlgorithm::PFF);
				print_info("Algoritmo de reemplazo de páginas cambiado a: PFF (Page Fault Frequency).");
			}
			else {
				print_error("Algoritmo no reconocido. Opciones: fifo, lru, pff.");
			}
		}
		else {
			print_error("'set_page_algo' requiere especificar el algoritmo (ej. set_page_algo lru).");
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
				print_error("'access' requiere una dirección virtual positiva (ej. access 2000).");
				return;
			}
		}
		else if (command_parts.size() == 3) {
			if (std::stringstream(command_parts[1]) >> address && address >= 0 &&
				std::stringstream(command_parts[2]) >> target_pid && target_pid > 0) {
			}
			else {
				print_error("Formato incorrecto. Uso: 'access <dir>' o 'access <dir> <pid>'.");
				return;
			}
		}
		else {
			print_error("'access' requiere una dirección (y opcionalmente un PID).");
			return;
		}

		if (target_pid == -1) {
			print_error("No hay un proceso en ejecución para acceder a la memoria (PID no encontrado).");
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
				print_error("'dreq' requiere un número de cilindro válido (0-199).");
			}
		}
		else if (command_parts.size() == 3) {
			if (std::stringstream(command_parts[1]) >> cylinder && cylinder >= 0 &&
				std::stringstream(command_parts[2]) >> target_pid && target_pid > 0) {
				kernel.disk_request(target_pid, cylinder);
			}
			else {
				print_error("Formato incorrecto. Uso: 'dreq <cilindro>' o 'dreq <cilindro> <pid>'.");
			}
		}
		else {
			print_error("'dreq' requiere al menos el número de cilindro.");
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
				print_info("Algoritmo de disco cambiado a: FCFS.");
			}
			else if (algo == "sstf") {
				kernel.set_disk_algorithm(DiskAlgorithm::SSTF);
				print_info("Algoritmo de disco cambiado a: SSTF.");
			}
			else if (algo == "scan") {
				kernel.set_disk_algorithm(DiskAlgorithm::SCAN);
				print_info("Algoritmo de disco cambiado a: SCAN.");
			}
			else {
				print_error("Algoritmo no reconocido. Opciones: fcfs, sstf, scan.");
			}
		}
		else {
			print_error("'set_disk_algo' requiere especificar el algoritmo (ej. set_disk_algo sstf).");
		}
	}
	else if (command == "ioreq") {
		if (command_parts.size() < 3) {
			print_error("Uso: ioreq <prioridad> <tiempo> [datos]");
			return;
		}

		int priority, service_time;
		if (!(std::stringstream(command_parts[1]) >> priority && priority >= 0) ||
			!(std::stringstream(command_parts[2]) >> service_time && service_time > 0)) {
			print_error("Prioridad y tiempo deben ser números válidos.");
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
	else if (command == "dlinit") {
		if (command_parts.size() != 5) {
			print_error("Uso: dlinit <pid> <r1> <r2> <r3>");
			return;
		}

		int pid, r1, r2, r3;
		if (std::stringstream(command_parts[1]) >> pid &&
			std::stringstream(command_parts[2]) >> r1 &&
			std::stringstream(command_parts[3]) >> r2 &&
			std::stringstream(command_parts[4]) >> r3) {
			kernel.deadlock_init_process(pid, { r1, r2, r3 });
		}
		else {
			print_error("Parámetros inválidos.");
		}
	}
	else if (command == "dlreq") {
		if (command_parts.size() != 5) {
			print_error("Uso: dlreq <pid> <r1> <r2> <r3>");
			return;
		}

		int pid, r1, r2, r3;
		if (std::stringstream(command_parts[1]) >> pid &&
			std::stringstream(command_parts[2]) >> r1 &&
			std::stringstream(command_parts[3]) >> r2 &&
			std::stringstream(command_parts[4]) >> r3) {
			kernel.deadlock_request(pid, { r1, r2, r3 });
		}
		else {
			print_error("Parámetros inválidos.");
		}
	}
	else if (command == "dlrel") {
		if (command_parts.size() != 5) {
			print_error("Uso: dlrel <pid> <r1> <r2> <r3>");
			return;
		}

		int pid, r1, r2, r3;
		if (std::stringstream(command_parts[1]) >> pid &&
			std::stringstream(command_parts[2]) >> r1 &&
			std::stringstream(command_parts[3]) >> r2 &&
			std::stringstream(command_parts[4]) >> r3) {
			kernel.deadlock_release(pid, { r1, r2, r3 });
		}
		else {
			print_error("Parámetros inválidos.");
		}
	}
	else if (command == "dldetect") {
		kernel.deadlock_detect();
	}
	else if (command == "dlstat") {
		kernel.print_deadlock_state();
	}
	else if (command == "dlstats") {
		kernel.print_deadlock_stats();
	}
	else if (command == "protect") {
		if (command_parts.size() != 4) {
			print_error("Uso: protect <pid> <addr> <read|write|exec>");
			return;
		}

		int pid, addr;
		if (std::stringstream(command_parts[1]) >> pid &&
			std::stringstream(command_parts[2]) >> std::hex >> addr) {
			std::string access_type = command_parts[3];
			std::transform(access_type.begin(), access_type.end(), access_type.begin(), ::tolower);
			kernel.check_memory_protection(pid, addr, access_type);
		}
		else {
			print_error("Parámetros inválidos.");
		}
	}
	else if (command == "syscall") {
		if (command_parts.size() < 3) {
			print_error("Uso: syscall <pid> <read|write|alloc|fork|exit> [args]");
			return;
		}

		int pid;
		if (std::stringstream(command_parts[1]) >> pid) {
			std::string syscall_type = command_parts[2];
			std::transform(syscall_type.begin(), syscall_type.end(), syscall_type.begin(), ::tolower);
			std::string args = (command_parts.size() > 3) ? command_parts[3] : "";
			kernel.invoke_syscall(pid, syscall_type, args);
		}
		else {
			print_error("PID inválido.");
		}
	}
	else if (command == "segments") {
		kernel.print_segments();
	}
	else if (command == "protstats") {
		kernel.print_protection_stats();
	}
	else if (command == "syscallstats") {
		kernel.print_syscall_stats();
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
			print_error("Uso: phil <start|stop|status|think|eat> [id]");
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
				print_error("ID de filósofo inválido (0-4).");
			}
		}
		else if (subcmd == "eat" && command_parts.size() == 3) {
			int id;
			if (std::stringstream(command_parts[2]) >> id && id >= 0 && id < 5) {
				kernel.philosopher_eat(id);
			}
			else {
				print_error("ID de filósofo inválido (0-4).");
			}
		}
		else {
			print_error("Subcomando desconocido. Uso: phil <start|stop|status|think|eat> [id]");
		}
	}
	else if (command == "kill") {
		int id;
		if (command_parts.size() > 1 && std::stringstream(command_parts[1]) >> id) {
			kernel.kill_process(id);
		}
		else {
			print_error("'kill' requiere el ID del proceso.");
		}
	}
	else if (command == "exec") {
		if (command_parts.size() != 2) {
			print_error("Uso: exec <archivo.txt>");
			return;
		}

		std::string script_file = command_parts[1];
		std::ifstream file(script_file);

		if (!file.is_open()) {
			print_error("No se pudo abrir: " + script_file);
			return;
		}

		std::string line;
		int executed = 0;

		print_info("Ejecutando script: " + script_file);

		while (std::getline(file, line)) {
			if (line.empty() || line[0] == '#') continue;

			std::cout << Colors::DIM << "  > " << line << Colors::RESET << std::endl;
			handle_command(line);
			executed++;
		}

		file.close();
		print_success(std::to_string(executed) + " comandos ejecutados.");
	}
	else if (command == "exit") {
		print_info("Saliendo del simulador. ¡Hasta pronto!");
		exit(0);
	}
	else if (command == "help") {
		print_header("\n========================================");
		print_header("    KERNEL-SIM v0.1 - AYUDA COMPLETA");
		print_header("========================================");

		print_section("\n[PROCESOS]");
		std::cout << colorize("new <burst>", Colors::YELLOW) << ": Crea un proceso con el tiempo de ráfaga especificado." << std::endl;
		std::cout << colorize("ps", Colors::YELLOW) << ": Lista todos los procesos y su estado." << std::endl;
		std::cout << colorize("kill <id>", Colors::YELLOW) << ": Termina el proceso con el ID especificado." << std::endl;

		print_section("\n[PLANIFICACIÓN CPU]");
		std::cout << colorize("run <n>", Colors::YELLOW) << ": Ejecuta 'n' ciclos de reloj." << std::endl;
		std::cout << colorize("set_algo <rr|sjf>", Colors::YELLOW) << ": Selecciona algoritmo de planificación." << std::endl;
		std::cout << colorize("set_quantum <n>", Colors::YELLOW) << ": Establece el Quantum para Round Robin." << std::endl;

		print_section("\n[MEMORIA]");
		std::cout << colorize("set_page_algo <fifo|lru|pff>", Colors::YELLOW) << ": Algoritmo de reemplazo de páginas." << std::endl;
		std::cout << colorize("access <dir> [pid]", Colors::YELLOW) << ": Simula acceso a memoria virtual." << std::endl;
		std::cout << colorize("memview", Colors::YELLOW) << ": Muestra estado de la memoria física." << std::endl;

		print_section("\n[DISCO]");
		std::cout << colorize("dreq <cilindro> [pid]", Colors::YELLOW) << ": Solicitud de acceso a disco." << std::endl;
		std::cout << colorize("dproc", Colors::YELLOW) << ": Procesa siguiente solicitud de disco." << std::endl;
		std::cout << colorize("dstat", Colors::YELLOW) << ": Muestra estado del planificador de disco." << std::endl;
		std::cout << colorize("disk_stats", Colors::YELLOW) << ": Muestra estadísticas de disco." << std::endl;
		std::cout << colorize("dview", Colors::YELLOW) << ": Visualiza posición del cabezal y solicitudes." << std::endl;
		std::cout << colorize("set_disk_algo <fcfs|sstf|scan>", Colors::YELLOW) << ": Cambia algoritmo de disco." << std::endl;

		print_section("\n[E/S]");
		std::cout << colorize("ioreq <prioridad> <tiempo> [datos]", Colors::YELLOW) << ": Solicitud de E/S." << std::endl;
		std::cout << colorize("ioproc", Colors::YELLOW) << ": Procesa operación de E/S." << std::endl;
		std::cout << colorize("iostat", Colors::YELLOW) << ": Estado del sistema de E/S." << std::endl;
		std::cout << colorize("io_stats", Colors::YELLOW) << ": Estadísticas de E/S." << std::endl;

		print_section("\n[DEADLOCK]");
		std::cout << colorize("dlinit <pid> <r1> <r2> <r3>", Colors::YELLOW) << ": Inicializa recursos máximos." << std::endl;
		std::cout << colorize("dlreq <pid> <r1> <r2> <r3>", Colors::YELLOW) << ": Solicita recursos." << std::endl;
		std::cout << colorize("dlrel <pid> <r1> <r2> <r3>", Colors::YELLOW) << ": Libera recursos." << std::endl;
		std::cout << colorize("dldetect", Colors::YELLOW) << ": Detecta deadlock." << std::endl;
		std::cout << colorize("dlstat", Colors::YELLOW) << ": Estado de recursos (Banker)." << std::endl;
		std::cout << colorize("dlstats", Colors::YELLOW) << ": Estadísticas de deadlock." << std::endl;

		print_section("\n[PROTECCIÓN]");
		std::cout << colorize("protect <pid> <addr> <read|write|exec>", Colors::YELLOW) << ": Verifica protección de memoria." << std::endl;
		std::cout << colorize("syscall <pid> <read|write|alloc|fork|exit> [args]", Colors::YELLOW) << ": Invoca syscall." << std::endl;
		std::cout << colorize("segments", Colors::YELLOW) << ": Muestra tabla de segmentos." << std::endl;
		std::cout << colorize("protstats", Colors::YELLOW) << ": Estadísticas de protección." << std::endl;
		std::cout << colorize("syscallstats", Colors::YELLOW) << ": Estadísticas de syscalls." << std::endl;

		print_section("\n[SINCRONIZACIÓN]");
		std::cout << colorize("produce [item]", Colors::YELLOW) << ": El proceso RUNNING produce un recurso." << std::endl;
		std::cout << colorize("consume", Colors::YELLOW) << ": El proceso RUNNING consume un recurso." << std::endl;
		std::cout << colorize("sync_stat", Colors::YELLOW) << ": Muestra estado del buffer P/C." << std::endl;

		print_section("\n[FILÓSOFOS]");
		std::cout << colorize("phil start", Colors::YELLOW) << ": Inicia simulación de filósofos." << std::endl;
		std::cout << colorize("phil stop", Colors::YELLOW) << ": Detiene simulación." << std::endl;
		std::cout << colorize("phil status", Colors::YELLOW) << ": Estado de los filósofos." << std::endl;
		std::cout << colorize("phil think <id>", Colors::YELLOW) << ": Filósofo ID piensa (0-4)." << std::endl;
		std::cout << colorize("phil eat <id>", Colors::YELLOW) << ": Filósofo ID intenta comer (0-4)." << std::endl;

		print_section("\n[SCRIPTS]");
		std::cout << colorize("exec <archivo>", Colors::YELLOW) << ": Ejecuta comandos desde un archivo de script." << std::endl;

		print_section("\n[GENERAL]");
		std::cout << colorize("stats", Colors::YELLOW) << ": Muestra todas las métricas de rendimiento." << std::endl;
		std::cout << colorize("exit", Colors::YELLOW) << ": Sale del simulador." << std::endl;

		print_header("\n========================================");
		print_header("       SCRIPTS DISPONIBLES");
		print_header("========================================");

		print_section("\n[MEMORIA]");
		std::cout << "scripts/mem_fifo.txt       - Experimento con FIFO" << std::endl;
		std::cout << "scripts/mem_lru.txt        - Experimento con LRU" << std::endl;
		std::cout << "scripts/mem_pff.txt        - Experimento con PFF" << std::endl;
		std::cout << "scripts/mem_trace.txt      - Traza básica de memoria" << std::endl;

		print_section("\n[DISCO]");
		std::cout << "scripts/disk_fcfs.txt      - Planificación FCFS" << std::endl;
		std::cout << "scripts/disk_sstf.txt      - Planificación SSTF" << std::endl;
		std::cout << "scripts/disk_scan.txt      - Planificación SCAN" << std::endl;
		std::cout << "scripts/disk_requests.txt  - Solicitudes de disco" << std::endl;

		print_section("\n[PROCESOS]");
		std::cout << "scripts/proc_rr.txt        - Round Robin" << std::endl;
		std::cout << "scripts/proc_sjf.txt       - Shortest Job First" << std::endl;

		print_section("\n[SINCRONIZACIÓN]");
		std::cout << "scripts/sync_philosophers.txt  - Cena de los Filósofos" << std::endl;

		print_section("\n[PROTECCIÓN Y SEGURIDAD]");
		std::cout << "scripts/protection_test.txt    - Sistema de protección" << std::endl;
		std::cout << "scripts/deadlock_banker.txt    - Algoritmo del Banquero" << std::endl;

		print_section("\n[DEMOSTRACIÓN COMPLETA]");
		std::cout << "scripts/demo_complete.txt      - Demo de todas las funciones" << std::endl;

		print_header("\n========================================");
		std::cout << colorize("Uso:", Colors::BOLD) << " exec scripts/mem_fifo.txt" << std::endl;
		print_header("========================================\n");
	}
	else {
		print_error("Comando desconocido. Use 'help'.");
	}
}