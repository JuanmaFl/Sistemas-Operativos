#ifndef KERNEL_CORE_H
#define KERNEL_CORE_H

#include <memory>
#include <vector>
#include <deque>
#include <string>

#include "../modules/mem/memory_manager.h"
#include "../modules/disk/disk_scheduler.h"
#include "process.h"
#include "scheduler.h"
#include "sync.h"
#include "philosophers.h"

const int PC_BUFFER_SIZE = 5;

class KernelSimulator {
private:
	std::unique_ptr<Scheduler> scheduler;
	std::unique_ptr<MemoryManager> memory_manager;
	std::unique_ptr<DiskScheduler> disk_scheduler;
	std::unique_ptr<DiningPhilosophers> dining_philosophers;
	int current_time;
	std::vector<std::shared_ptr<Process>> all_processes;

	std::deque<std::string> pc_buffer;
	Semaphore mutex = Semaphore(1);
	Semaphore empty = Semaphore(PC_BUFFER_SIZE);
	Semaphore full = Semaphore(0);

public:
	KernelSimulator();
	~KernelSimulator();
	void print_stats();

	void run_tick();
	void run_cycles(int n);

	void new_process(int burst);
	void list_processes();
	bool kill_process(int pid);

	void set_scheduler_algorithm(SchedulerAlgorithm algo) {
		if (scheduler) {
			scheduler->set_algorithm(algo);
		}
	}
	void set_quantum(int quantum);
	void set_replacement_algorithm(ReplacementAlgorithm algo) {
		if (memory_manager) {
			memory_manager->set_replacement_algorithm(algo);
		}
	}
	void set_disk_algorithm(DiskAlgorithm algo) {
		if (disk_scheduler) {
			disk_scheduler->set_algorithm(algo);
		}
	}

	void access_memory(int pid, int virtual_address);
	void access_memory_cli(int virtual_address);
	void print_memory_status();
	void print_memory_stats();

	void disk_request(int cylinder);
	void disk_request(int pid, int cylinder);
	void process_disk_request();
	void print_disk_status() const;
	void print_disk_stats() const;
	void visualize_disk() const;

	void start_philosophers();
	void stop_philosophers();
	void philosopher_think(int id);
	void philosopher_eat(int id);
	void print_philosophers_status() const;

	int get_running_process_id() const;

	void produce_resource(const std::string& item);
	void consume_resource();
	void print_pc_status() const;

private:
	void check_for_interrupts();
	std::shared_ptr<Process> get_process_by_id(int pid);
};

#endif