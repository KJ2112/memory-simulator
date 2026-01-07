#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>

struct Block {
    size_t address;  // start offset in physical memory
    size_t size;     // length of this block
    bool is_free;    // true if free, false if allocated
    int id;          // block ID for tracking allocations
    
    Block(size_t addr = 0, size_t sz = 0, bool free = true, int block_id = -1)
        : address(addr), size(sz), is_free(free), id(block_id) {}
};

#endif // BLOCK_H