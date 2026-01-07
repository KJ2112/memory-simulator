#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <vector>
#include <queue>
#include <cstddef>

struct PageTableEntry {
    bool valid;
    int frame;
    size_t timestamp;  // For LRU
    
    PageTableEntry() : valid(false), frame(-1), timestamp(0) {}
};

enum PageReplacementPolicy {
    PAGE_FIFO,
    PAGE_LRU
};

class VirtualMemory {
public:
    VirtualMemory(size_t num_pages, size_t page_size, size_t num_frames,
                  PageReplacementPolicy policy = PAGE_FIFO);
    
    size_t translate(size_t virtual_address);
    void stats() const;
    void reset();
    void setPolicy(PageReplacementPolicy policy);
    
private:
    size_t num_pages;
    size_t page_size;
    size_t num_frames;
    PageReplacementPolicy policy;
    
    std::vector<PageTableEntry> page_table;
    std::vector<bool> frame_used;
    std::queue<int> fifo_queue;
    size_t time_counter;
    
    size_t page_faults;
    size_t page_hits;
    
    // Helper methods
    int findFreeFrame();
    int selectVictimPage();
    void handlePageFault(size_t page_num);
};

#endif // VIRTUAL_MEMORY_H