#include "MemoryManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

MemoryManager::MemoryManager()
    : total_memory(0), used_memory(0), next_id(1),
      current_strategy(FIRST_FIT), internal_frag(0),
      total_alloc_requests(0), failed_requests(0) {}

void MemoryManager::init(size_t total_size) {
    blocks.clear();
    blocks.push_back(Block(0, total_size, true, -1));
    total_memory = total_size;
    used_memory = 0;
    next_id = 1;
    internal_frag = 0;
    total_alloc_requests = 0;
    failed_requests = 0;
    std::cout << "Memory initialized: " << total_size << " bytes\n";
}

void MemoryManager::setStrategy(AllocStrategy strategy) {
    current_strategy = strategy;
    std::string stratName;
    switch(strategy) {
        case FIRST_FIT: stratName = "First Fit"; break;
        case BEST_FIT: stratName = "Best Fit"; break;
        case WORST_FIT: stratName = "Worst Fit"; break;
    }
    std::cout << "Allocation strategy set to: " << stratName << "\n";
}

int MemoryManager::malloc(size_t nbytes) {
    total_alloc_requests++;
    
    if (nbytes == 0) {
        std::cout << "Error: Cannot allocate 0 bytes\n";
        failed_requests++;
        return -1;
    }
    
    int block_index = -1;
    switch(current_strategy) {
        case FIRST_FIT:
            block_index = firstFit(nbytes);
            break;
        case BEST_FIT:
            block_index = bestFit(nbytes);
            break;
        case WORST_FIT:
            block_index = worstFit(nbytes);
            break;
    }
    
    if (block_index == -1) {
        std::cout << "Error: Allocation failed - not enough memory\n";
        failed_requests++;
        return -1;
    }
    
    Block& chosen = blocks[block_index];
    
    // Track internal fragmentation if block is larger than needed
    if (chosen.size > nbytes) {
        internal_frag += (chosen.size - nbytes);
    }
    
    // Split block if necessary
    if (chosen.size > nbytes) {
        Block new_block(chosen.address + nbytes, chosen.size - nbytes, true, -1);
        chosen.size = nbytes;
        blocks.insert(blocks.begin() + block_index + 1, new_block);
    }
    
    // Allocate the block
    chosen.is_free = false;
    chosen.id = next_id++;
    used_memory += chosen.size;
    
    std::cout << "Allocated block id=" << chosen.id 
              << " at address=0x" << std::hex << std::setfill('0') 
              << std::setw(4) << chosen.address << std::dec << "\n";
    
    return chosen.id;
}

void MemoryManager::free(int block_id) {
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].id == block_id && !blocks[i].is_free) {
            blocks[i].is_free = true;
            blocks[i].id = -1;
            used_memory -= blocks[i].size;
            
            // Coalesce with adjacent free blocks
            coalesce(i);
            
            std::cout << "Block " << block_id << " freed and merged\n";
            return;
        }
    }
    std::cout << "Error: Block " << block_id << " not found or already free\n";
}

void MemoryManager::coalesce(size_t index) {
    // Merge with next block if it's free
    while (index + 1 < blocks.size() && blocks[index + 1].is_free) {
        blocks[index].size += blocks[index + 1].size;
        blocks.erase(blocks.begin() + index + 1);
    }
    
    // Merge with previous block if it's free
    while (index > 0 && blocks[index - 1].is_free) {
        blocks[index - 1].size += blocks[index].size;
        blocks.erase(blocks.begin() + index);
        index--;
    }
}

int MemoryManager::firstFit(size_t size) {
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].is_free && blocks[i].size >= size) {
            return i;
        }
    }
    return -1;
}

int MemoryManager::bestFit(size_t size) {
    int best_index = -1;
    size_t best_size = SIZE_MAX;
    
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].is_free && blocks[i].size >= size) {
            if (blocks[i].size < best_size) {
                best_size = blocks[i].size;
                best_index = i;
            }
        }
    }
    return best_index;
}

int MemoryManager::worstFit(size_t size) {
    int worst_index = -1;
    size_t worst_size = 0;
    
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].is_free && blocks[i].size >= size) {
            if (blocks[i].size > worst_size) {
                worst_size = blocks[i].size;
                worst_index = i;
            }
        }
    }
    return worst_index;
}

void MemoryManager::dump() const {
    std::cout << "\n=== Memory Dump ===\n";
    for (const auto& block : blocks) {
        std::cout << "[0x" << std::hex << std::setfill('0') << std::setw(4) 
                  << block.address << " - 0x" << std::setw(4) 
                  << (block.address + block.size - 1) << std::dec << "] ";
        
        if (block.is_free) {
            std::cout << "FREE";
        } else {
            std::cout << "USED (id=" << block.id << ")";
        }
        std::cout << " [" << block.size << " bytes]\n";
    }
    std::cout << "===================\n\n";
}

size_t MemoryManager::getLargestFreeBlock() const {
    size_t largest = 0;
    for (const auto& block : blocks) {
        if (block.is_free && block.size > largest) {
            largest = block.size;
        }
    }
    return largest;
}

size_t MemoryManager::calculateExternalFragmentation() const {
    size_t total_free = 0;
    for (const auto& block : blocks) {
        if (block.is_free) {
            total_free += block.size;
        }
    }
    
    size_t largest_free = getLargestFreeBlock();
    if (total_free == 0) return 0;
    
    return ((total_free - largest_free) * 100) / total_free;
}

void MemoryManager::stats() const {
    std::cout << "\n=== Memory Statistics ===\n";
    std::cout << "Total memory: " << total_memory << " bytes\n";
    std::cout << "Used memory: " << used_memory << " bytes\n";
    std::cout << "Free memory: " << (total_memory - used_memory) << " bytes\n";
    std::cout << "Memory utilization: " 
              << (total_memory > 0 ? (used_memory * 100) / total_memory : 0) << "%\n";
    std::cout << "Internal fragmentation: " << internal_frag << " bytes\n";
    std::cout << "External fragmentation: " << calculateExternalFragmentation() << "%\n";
    std::cout << "Allocation requests: " << total_alloc_requests << "\n";
    std::cout << "Failed requests: " << failed_requests << "\n";
    std::cout << "Success rate: " 
              << (total_alloc_requests > 0 ? 
                  ((total_alloc_requests - failed_requests) * 100) / total_alloc_requests : 0)
              << "%\n";
    std::cout << "========================\n\n";
}