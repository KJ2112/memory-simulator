#include "BuddyAllocator.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

BuddyAllocator::BuddyAllocator()
    : total_memory(0), used_memory(0), next_id(1),
      total_alloc_requests(0), failed_requests(0), internal_frag(0) {}

bool BuddyAllocator::isPowerOfTwo(size_t n) const {
    return n > 0 && (n & (n - 1)) == 0;
}

size_t BuddyAllocator::nextPowerOfTwo(size_t n) const {
    if (n == 0) return 1;
    if (isPowerOfTwo(n)) return n;
    
    size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

void BuddyAllocator::init(size_t total_size) {
    if (!isPowerOfTwo(total_size)) {
        std::cout << "Error: Total size must be a power of 2\n";
        return;
    }
    
    free_lists.clear();
    allocated_blocks.clear();
    total_memory = total_size;
    used_memory = 0;
    next_id = 1;
    total_alloc_requests = 0;
    failed_requests = 0;
    internal_frag = 0;
    
    // Add the entire memory as one free block
    free_lists[total_size].push_back(0);
    
    std::cout << "Buddy allocator initialized: " << total_size << " bytes\n";
}

size_t BuddyAllocator::getBuddyAddress(size_t address, size_t size) const {
    return address ^ size;
}

void BuddyAllocator::splitBlock(size_t address, size_t current_size, size_t target_size) {
    while (current_size > target_size) {
        current_size /= 2;
        size_t buddy_addr = address + current_size;
        free_lists[current_size].push_back(buddy_addr);
    }
}

int BuddyAllocator::allocate(size_t size) {
    total_alloc_requests++;
    
    if (size == 0) {
        std::cout << "Error: Cannot allocate 0 bytes\n";
        failed_requests++;
        return -1;
    }
    
    // Round up to next power of two
    size_t actual_size = nextPowerOfTwo(size);
    
    // Track internal fragmentation
    internal_frag += (actual_size - size);
    
    // Find the smallest available block that fits
    size_t block_size = actual_size;
    while (block_size <= total_memory && free_lists[block_size].empty()) {
        block_size *= 2;
    }
    
    if (block_size > total_memory) {
        std::cout << "Error: Allocation failed - not enough memory\n";
        failed_requests++;
        return -1;
    }
    
    // Get a block of the found size
    size_t address = free_lists[block_size].front();
    free_lists[block_size].pop_front();
    
    // Split the block down to the required size
    splitBlock(address, block_size, actual_size);
    
    // Allocate the block
    int id = next_id++;
    allocated_blocks[id] = BuddyBlock(address, actual_size, id);
    used_memory += actual_size;
    
    std::cout << "Allocated block id=" << id 
              << " at address=0x" << std::hex << std::setfill('0') 
              << std::setw(4) << address << std::dec 
              << " (requested: " << size << ", actual: " << actual_size << " bytes)\n";
    
    return id;
}

void BuddyAllocator::free(int block_id) {
    auto it = allocated_blocks.find(block_id);
    if (it == allocated_blocks.end()) {
        std::cout << "Error: Block " << block_id << " not found\n";
        return;
    }
    
    BuddyBlock block = it->second;
    allocated_blocks.erase(it);
    used_memory -= block.size;
    
    size_t address = block.address;
    size_t size = block.size;
    
    // Try to merge with buddy repeatedly
    while (size < total_memory) {
        size_t buddy_addr = getBuddyAddress(address, size);
        
        // Check if buddy is free
        auto& free_list = free_lists[size];
        auto buddy_it = std::find(free_list.begin(), free_list.end(), buddy_addr);
        
        if (buddy_it != free_list.end()) {
            // Buddy is free, merge
            free_list.erase(buddy_it);
            address = std::min(address, buddy_addr);
            size *= 2;
        } else {
            // Buddy is not free, stop merging
            break;
        }
    }
    
    // Add the merged block to free list
    free_lists[size].push_back(address);
    
    std::cout << "Block " << block_id << " freed and merged\n";
}

void BuddyAllocator::dump() const {
    std::cout << "\n=== Buddy Allocator Memory Dump ===\n";
    
    // Show allocated blocks
    std::cout << "Allocated blocks:\n";
    for (const auto& pair : allocated_blocks) {
        const BuddyBlock& block = pair.second;
        std::cout << "  [0x" << std::hex << std::setfill('0') << std::setw(4) 
                  << block.address << " - 0x" << std::setw(4) 
                  << (block.address + block.size - 1) << std::dec 
                  << "] USED (id=" << block.id << ", " << block.size << " bytes)\n";
    }
    
    // Show free blocks
    std::cout << "Free blocks:\n";
    for (const auto& pair : free_lists) {
        size_t size = pair.first;
        const auto& list = pair.second;
        for (size_t addr : list) {
            std::cout << "  [0x" << std::hex << std::setfill('0') << std::setw(4) 
                      << addr << " - 0x" << std::setw(4) 
                      << (addr + size - 1) << std::dec 
                      << "] FREE (" << size << " bytes)\n";
        }
    }
    std::cout << "===================================\n\n";
}

void BuddyAllocator::stats() const {
    std::cout << "\n=== Buddy Allocator Statistics ===\n";
    std::cout << "Total memory: " << total_memory << " bytes\n";
    std::cout << "Used memory: " << used_memory << " bytes\n";
    std::cout << "Free memory: " << (total_memory - used_memory) << " bytes\n";
    std::cout << "Memory utilization: " 
              << (total_memory > 0 ? (used_memory * 100) / total_memory : 0) << "%\n";
    std::cout << "Internal fragmentation: " << internal_frag << " bytes\n";
    std::cout << "Allocation requests: " << total_alloc_requests << "\n";
    std::cout << "Failed requests: " << failed_requests << "\n";
    std::cout << "Success rate: " 
              << (total_alloc_requests > 0 ? 
                  ((total_alloc_requests - failed_requests) * 100) / total_alloc_requests : 0)
              << "%\n";
    std::cout << "==================================\n\n";
}