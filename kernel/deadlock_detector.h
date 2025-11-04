#ifndef KERNEL_DEADLOCK_DETECTOR_H
#define KERNEL_DEADLOCK_DETECTOR_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include "process.h"

const int NUM_RESOURCE_TYPES = 3;

struct ResourceState {
    std::vector<int> available;
    std::vector<std::vector<int>> max_demand;
    std::vector<std::vector<int>> allocation;
    std::vector<std::vector<int>> need;
};

class DeadlockDetector {
private:
    ResourceState state;
    int num_processes;

    int total_detections = 0;
    int safe_states = 0;
    int unsafe_states = 0;

public:
    DeadlockDetector();

    void initialize_process(int pid, const std::vector<int>& max_resources);
    bool request_resources(int pid, const std::vector<int>& request);
    void release_resources(int pid, const std::vector<int>& release);

    bool is_safe_state();
    std::vector<int> find_safe_sequence();

    void detect_deadlock();
    void print_resource_state() const;
    void print_deadlock_stats() const;

private:
    bool banker_algorithm(const std::vector<int>& work, std::vector<bool>& finish, std::vector<int>& sequence);
    void calculate_need();
};

#endif