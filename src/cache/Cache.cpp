#include "Cache.h"
#include <iostream>
#include <iomanip>

Cache::Cache(const std::string& name, size_t cache_size, size_t block_size, 
             size_t associativity, ReplacementPolicy policy)
    : name(name), cache_size(cache_size), block_size(block_size),
      associativity(associativity), policy(policy),
      hits(0), misses(0), time_counter(0) {
    
    // Calculate number of sets
    size_t total_lines = cache_size / block_size;
    num_sets = total_lines / associativity;
    
    // Initialize sets
    sets.resize(num_sets);
    for (auto& set : sets) {
        set.resize(associativity);
    }
    
    std::cout << name << " initialized: " << cache_size << " bytes, "
              << block_size << " byte blocks, " 
              << associativity << "-way associative, "
              << num_sets << " sets\n";
}

size_t Cache::getSetIndex(size_t address) const {
    size_t block_number = address / block_size;
    return block_number % num_sets;
}

size_t Cache::getTag(size_t address) const {
    size_t block_number = address / block_size;
    return block_number / num_sets;
}

int Cache::findLine(size_t set_index, size_t tag) const {
    for (size_t i = 0; i < associativity; i++) {
        if (sets[set_index][i].valid && sets[set_index][i].tag == tag) {
            return i;
        }
    }
    return -1;
}

int Cache::findVictim(size_t set_index) {
    // First, try to find an invalid line
    for (size_t i = 0; i < associativity; i++) {
        if (!sets[set_index][i].valid) {
            return i;
        }
    }
    
    // All lines are valid, use replacement policy
    if (policy == FIFO || policy == LRU) {
        // Find line with smallest timestamp
        size_t victim = 0;
        size_t min_time = sets[set_index][0].timestamp;
        
        for (size_t i = 1; i < associativity; i++) {
            if (sets[set_index][i].timestamp < min_time) {
                min_time = sets[set_index][i].timestamp;
                victim = i;
            }
        }
        return victim;
    }
    
    return 0;
}

bool Cache::access(size_t address) {
    time_counter++;
    
    size_t set_index = getSetIndex(address);
    size_t tag = getTag(address);
    
    // Check for hit
    int line_index = findLine(set_index, tag);
    if (line_index != -1) {
        // Cache hit
        hits++;
        
        // Update timestamp for LRU
        if (policy == LRU) {
            sets[set_index][line_index].timestamp = time_counter;
        }
        
        return true;
    }
    
    // Cache miss
    misses++;
    
    // Find victim and replace
    int victim = findVictim(set_index);
    sets[set_index][victim].valid = true;
    sets[set_index][victim].tag = tag;
    sets[set_index][victim].timestamp = time_counter;
    
    return false;
}

void Cache::reset() {
    hits = 0;
    misses = 0;
    time_counter = 0;
    
    for (auto& set : sets) {
        for (auto& line : set) {
            line.valid = false;
            line.tag = 0;
            line.timestamp = 0;
        }
    }
}

void Cache::stats() const {
    size_t total_accesses = hits + misses;
    double hit_ratio = total_accesses > 0 ? (double)hits / total_accesses * 100.0 : 0.0;
    
    std::cout << "\n=== " << name << " Statistics ===\n";
    std::cout << "Total accesses: " << total_accesses << "\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Misses: " << misses << "\n";
    std::cout << "Hit ratio: " << std::fixed << std::setprecision(2) 
              << hit_ratio << "%\n";
    std::cout << "Miss ratio: " << std::fixed << std::setprecision(2) 
              << (100.0 - hit_ratio) << "%\n";
    std::cout << "============================\n\n";
}