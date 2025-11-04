#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <iostream>
#include <vector>
#include <string>
#include "../modules/mem/page_table.h"

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

enum class PrivilegeLevel {
    KERNEL = 0,
    USER = 3
};

class Process {
public:
    int id;
    ProcessState state;
    int arrival_time;
    int total_burst_time;
    int remaining_time;
    int wait_time;
    int turnaround_time;
    int time_on_cpu;
    PageTable page_table;

    PrivilegeLevel privilege_level;
    int syscall_count;
    int protection_violations;

    Process(int pid, int burst, int arrival = 0, PrivilegeLevel level = PrivilegeLevel::USER)
        : id(pid),
        state(ProcessState::NEW),
        arrival_time(arrival),
        total_burst_time(burst),
        remaining_time(burst),
        wait_time(0),
        turnaround_time(0),
        time_on_cpu(0),
        privilege_level(level),
        syscall_count(0),
        protection_violations(0)
    {
    }

    void run_tick() {
        if (remaining_time > 0) {
            remaining_time--;
            time_on_cpu++;
        }
    }

    void reset_quantum_counter() {
        time_on_cpu = 0;
    }

    bool is_kernel_mode() const {
        return privilege_level == PrivilegeLevel::KERNEL;
    }

    void elevate_privilege() {
        privilege_level = PrivilegeLevel::KERNEL;
    }

    void lower_privilege() {
        privilege_level = PrivilegeLevel::USER;
    }
};

#endif