#include "heap_allocator.h"
#include <iostream>
#include <iomanip>
#include <cmath>

BuddyAllocator::BuddyAllocator(size_t total_size, size_t min_block_size)
    : total_size(total_size), min_block_size(min_block_size) {

    Block initial_block;
    initial_block.size = total_size;
    initial_block.is_free = true;
    initial_block.address = nullptr;
    initial_block.buddy = nullptr;

    blocks.push_back(initial_block);
    free_lists[total_size].push_back(&blocks[0]);

    std::cout << "[HEAP] Buddy Allocator inicializado (" << total_size << " bytes)" << std::endl;
}

size_t BuddyAllocator::next_power_of_two(size_t n) const {
    if (n <= min_block_size) return min_block_size;

    size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

void* BuddyAllocator::allocate(size_t size) {
    size_t required_size = next_power_of_two(size);

    for (auto& pair : free_lists) {
        if (pair.first >= required_size && !pair.second.empty()) {
            Block* block = pair.second.back();
            pair.second.pop_back();

            while (block->size > required_size && block->size / 2 >= min_block_size) {
                split_block(block, block->size / 2);
            }

            block->is_free = false;
            total_allocated += block->size;
            allocation_count++;

            std::cout << "[HEAP] Asignado: " << block->size << " bytes" << std::endl;
            return block->address;
        }
    }

    std::cout << "[HEAP] ERROR: No hay espacio suficiente para " << size << " bytes" << std::endl;
    return nullptr;
}

void BuddyAllocator::deallocate(void* ptr) {
    for (auto& block : blocks) {
        if (block.address == ptr && !block.is_free) {
            block.is_free = true;
            total_freed += block.size;
            free_count++;

            free_lists[block.size].push_back(&block);
            merge_buddies(&block);

            std::cout << "[HEAP] Liberado: " << block.size << " bytes" << std::endl;
            return;
        }
    }

    std::cout << "[HEAP] ERROR: Dirección inválida" << std::endl;
}

void BuddyAllocator::split_block(Block* block, size_t target_size) {
    Block new_block;
    new_block.size = block->size / 2;
    new_block.is_free = true;
    new_block.buddy = block;

    block->size = block->size / 2;
    block->buddy = &blocks.back();

    blocks.push_back(new_block);
    free_lists[new_block.size].push_back(&blocks.back());
}

void BuddyAllocator::merge_buddies(Block* block) {
    if (block->buddy && block->buddy->is_free && block->size == block->buddy->size) {
        block->size *= 2;
        block->buddy->is_free = false;

        std::cout << "[HEAP] Fusionando buddies: " << block->size << " bytes" << std::endl;
    }
}

double BuddyAllocator::get_fragmentation() const {
    size_t used = total_allocated - total_freed;
    if (total_size == 0) return 0.0;
    return (static_cast<double>(total_size - used) / total_size) * 100.0;
}

void BuddyAllocator::print_status() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     ESTADO DEL HEAP (Buddy System)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Tamaño Total: " << total_size << " bytes" << std::endl;
    std::cout << "Asignado: " << (total_allocated - total_freed) << " bytes" << std::endl;
    std::cout << "Fragmentación: " << std::fixed << std::setprecision(2)
        << get_fragmentation() << "%" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void BuddyAllocator::print_stats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     ESTADÍSTICAS DE HEAP" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total Asignaciones: " << allocation_count << std::endl;
    std::cout << "Total Liberaciones: " << free_count << std::endl;
    std::cout << "Bytes Asignados: " << total_allocated << std::endl;
    std::cout << "Bytes Liberados: " << total_freed << std::endl;
    std::cout << "Fragmentación: " << std::fixed << std::setprecision(2)
        << get_fragmentation() << "%" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

