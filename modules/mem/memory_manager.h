#ifndef MEM_MEMORY_MANAGER_H
#define MEM_MEMORY_MANAGER_H

#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>
#include <limits>
#include <map>
#include "page_table.h"

const int FRAME_COUNT = 16;
const int FRAME_SIZE = 256;

enum class ReplacementAlgorithm {
    FIFO,
    LRU,
    PFF
};

struct Frame {
    int frame_id;
    int pid = -1;
    int page_number = -1;
    int load_time = 0;
    int last_access_time = 0;
};

struct PFFState {
    int allocated_frames = 0;
    int page_faults = 0;
    int last_fault_time = 0;
    double fault_rate = 0.0;
};

class MemoryManager {
private:
    std::vector<Frame> physical_memory;
    std::deque<int> fifo_queue;
    ReplacementAlgorithm current_algorithm = ReplacementAlgorithm::FIFO;

    std::map<int, PFFState> pff_states;
    const int PFF_WINDOW = 10;
    const double PFF_UPPER_THRESHOLD = 0.5;
    const double PFF_LOWER_THRESHOLD = 0.2;

    int total_accesses = 0;
    int total_hits = 0;
    int total_faults = 0;

public:
    MemoryManager();

    void access_memory(int pid, int virtual_address, PageTable& page_table, int current_time);
    void print_memory_status() const;
    void print_memory_stats() const;

    void set_replacement_algorithm(ReplacementAlgorithm algo);

private:
    int get_free_frame();
    int find_frame_to_replace(PageTable& page_table, int pid, int current_time);
    void evict_frame(int victim_frame_id, int current_time, PageTable& page_table);

    int find_frame_pff(int pid, int current_time);
    void update_pff_state(int pid, int current_time, bool page_fault);
    std::string get_algorithm_name() const;
};

#endif