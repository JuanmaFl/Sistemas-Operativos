#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include <vector>
#include <map>
#include <cstddef>

struct Block {
    size_t size;
    bool is_free;
    void* address;
    Block* buddy;
};

class BuddyAllocator {
private:
    size_t total_size;
    size_t min_block_size;
    std::vector<Block> blocks;
    std::map<size_t, std::vector<Block*>> free_lists;

    size_t total_allocated = 0;
    size_t total_freed = 0;
    int allocation_count = 0;
    int free_count = 0;

public:
    BuddyAllocator(size_t total_size, size_t min_block_size);

    void* allocate(size_t size);
    void deallocate(void* ptr);

    void print_status() const;
    void print_stats() const;
    double get_fragmentation() const;

private:
    size_t next_power_of_two(size_t n) const;
    Block* find_buddy(Block* block);
    void split_block(Block* block, size_t target_size);
    void merge_buddies(Block* block);
};

#endif