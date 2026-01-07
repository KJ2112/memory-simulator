#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <map>
#include <list>
#include <cstddef>

struct BuddyBlock {
    size_t address;
    size_t size;
    int id;
    
    BuddyBlock(size_t addr = 0, size_t sz = 0, int block_id = -1)
        : address(addr), size(sz), id(block_id) {}
};

class BuddyAllocator {
public:
    BuddyAllocator();
    
    void init(size_t total_size);
    int allocate(size_t size);
    void free(int block_id);
    void dump() const;
    void stats() const;
    
private:
    std::map<size_t, std::list<size_t>> free_lists;  // free lists keyed by block size
    std::map<int, BuddyBlock> allocated_blocks;      // track allocated blocks by ID
    size_t total_memory;
    size_t used_memory;
    int next_id;
    
    // Statistics
    size_t total_alloc_requests;
    size_t failed_requests;
    size_t internal_frag;
    
    // Helper methods
    size_t nextPowerOfTwo(size_t n) const;
    bool isPowerOfTwo(size_t n) const;
    size_t getBuddyAddress(size_t address, size_t size) const;
    void splitBlock(size_t address, size_t current_size, size_t target_size);
};

#endif // BUDDY_ALLOCATOR_H