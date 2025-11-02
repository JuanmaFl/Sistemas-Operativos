// kernel/process.cpp

#include "process.h"

Process::Process(int id, int burst, int arrival)
    : id(id),
    state(ProcessState::NEW),
    total_burst_time(burst),
    remaining_time(burst),
    time_on_cpu(0),
    arrival_time(arrival),
    wait_time(0),
    turnaround_time(0) {
}

void Process::run_tick() {
    remaining_time--;
    time_on_cpu++;
}

void Process::reset_quantum_counter() {
    time_on_cpu = 0;
}