#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "Block.h"
#include <vector>
#include <string>

enum AllocStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

class MemoryManager {
public:
    MemoryManager();
    
    void init(size_t total_size);
    int malloc(size_t nbytes);
    void free(int block_id);
    void dump() const;
    void stats() const;
    void setStrategy(AllocStrategy strategy);
    
private:
    std::vector<Block> blocks;
    size_t total_memory;
    size_t used_memory;
    int next_id;
    AllocStrategy current_strategy;
    
    // Statistics
    size_t internal_frag;
    size_t total_alloc_requests;
    size_t failed_requests;
    
    // Helper methods
    int firstFit(size_t size);
    int bestFit(size_t size);
    int worstFit(size_t size);
    void coalesce(size_t index);
    size_t calculateExternalFragmentation() const;
    size_t getLargestFreeBlock() const;
};

#endif // MEMORY_MANAGER_H