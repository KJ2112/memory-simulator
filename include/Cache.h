#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstddef>
#include <string>

struct CacheLine {
    bool valid;
    size_t tag;
    size_t timestamp;  // For LRU or FIFO
    
    CacheLine() : valid(false), tag(0), timestamp(0) {}
};

enum ReplacementPolicy {
    FIFO,
    LRU
};

class Cache {
public:
    Cache(const std::string& name, size_t cache_size, size_t block_size, 
          size_t associativity, ReplacementPolicy policy = FIFO);
    
    bool access(size_t address);
    void stats() const;
    void reset();
    std::string getName() const { return name; }
    
private:
    std::string name;
    size_t cache_size;
    size_t block_size;
    size_t associativity;
    size_t num_sets;
    ReplacementPolicy policy;
    
    std::vector<std::vector<CacheLine>> sets;
    
    size_t hits;
    size_t misses;
    size_t time_counter;
    
    // Helper methods
    size_t getSetIndex(size_t address) const;
    size_t getTag(size_t address) const;
    int findLine(size_t set_index, size_t tag) const;
    int findVictim(size_t set_index);
};

#endif // CACHE_H