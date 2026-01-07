#include "VirtualMemory.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

VirtualMemory::VirtualMemory(size_t num_pages, size_t page_size, size_t num_frames,
                             PageReplacementPolicy policy)
    : num_pages(num_pages), page_size(page_size), num_frames(num_frames),
      policy(policy), time_counter(0), page_faults(0), page_hits(0) {
    
    page_table.resize(num_pages);
    frame_used.resize(num_frames, false);
    
    std::cout << "Virtual memory initialized: " << num_pages << " pages, "
              << page_size << " bytes per page, " << num_frames << " frames\n";
}

void VirtualMemory::setPolicy(PageReplacementPolicy new_policy) {
    policy = new_policy;
    std::string policyName = (policy == PAGE_FIFO) ? "FIFO" : "LRU";
    std::cout << "Page replacement policy set to: " << policyName << "\n";
}

int VirtualMemory::findFreeFrame() {
    for (size_t i = 0; i < num_frames; i++) {
        if (!frame_used[i]) {
            return i;
        }
    }
    return -1;
}

int VirtualMemory::selectVictimPage() {
    if (policy == PAGE_FIFO) {
        // Use FIFO queue
        if (!fifo_queue.empty()) {
            int victim_page = fifo_queue.front();
            fifo_queue.pop();
            return victim_page;
        }
    } else if (policy == PAGE_LRU) {
        // Find page with smallest timestamp
        int victim_page = -1;
        size_t min_time = SIZE_MAX;
        
        for (size_t i = 0; i < num_pages; i++) {
            if (page_table[i].valid && page_table[i].timestamp < min_time) {
                min_time = page_table[i].timestamp;
                victim_page = i;
            }
        }
        return victim_page;
    }
    return -1;
}

bool VirtualMemory::handlePageFault(size_t page_num) {
    page_faults++;
    
    // Try to find a free frame
    int frame = findFreeFrame();
    
    if (frame == -1) { // No free frame
        int victim_page = selectVictimPage();
        
        if (victim_page == -1) {
            std::cout << "Error: Cannot find victim page and no free frames\n";
            return false; // Failed to handle page fault
        }
        
        frame = page_table[victim_page].frame;
        page_table[victim_page].valid = false;
        page_table[victim_page].frame = -1;
        
        std::cout << "Page fault: evicting page " << victim_page 
                  << " from frame " << frame << "\n";
    }
    
    // At this point, 'frame' must be a valid frame number.

    frame_used[frame] = true;
    page_table[page_num].valid = true;
    page_table[page_num].frame = frame;
    page_table[page_num].timestamp = time_counter++;
    
    if (policy == PAGE_FIFO) {
        fifo_queue.push(page_num);
    }
    
    std::cout << "Page fault: loading page " << page_num 
              << " into frame " << frame << "\n";

    return true; // Successfully handled page fault
}

size_t VirtualMemory::translate(size_t virtual_address) {
    size_t page_num = virtual_address / page_size;
    size_t offset = virtual_address % page_size;
    
    if (page_num >= num_pages) {
        std::cout << "Error: Invalid virtual address 0x" << std::hex 
                  << virtual_address << std::dec << "\n";
        return SIZE_MAX; // Return error code for invalid address
    }
    
    if (page_table[page_num].valid) {
        // Page hit
        page_hits++;
        
        // Update timestamp for LRU
        if (policy == PAGE_LRU) {
            page_table[page_num].timestamp = time_counter++;
        }
        
        int frame = page_table[page_num].frame;
        size_t physical_address = frame * page_size + offset;
        
        return physical_address;
    } else {
        // Page fault
        if (!handlePageFault(page_num)) {
            // If handling the page fault failed, return an error
            return SIZE_MAX; 
        }
        
        // Page fault handled successfully, now we can translate
        int frame = page_table[page_num].frame;
        size_t physical_address = frame * page_size + offset;
        
        return physical_address;
    }
}

void VirtualMemory::reset() {
    for (auto& entry : page_table) {
        entry.valid = false;
        entry.frame = -1;
        entry.timestamp = 0;
    }
    
    for (size_t i = 0; i < num_frames; i++) {
        frame_used[i] = false;
    }
    
    while (!fifo_queue.empty()) {
        fifo_queue.pop();
    }
    
    time_counter = 0;
    page_faults = 0;
    page_hits = 0;
}

void VirtualMemory::stats() const {
    size_t total_accesses = page_hits + page_faults;
    double hit_ratio = total_accesses > 0 ? 
                       (double)page_hits / total_accesses * 100.0 : 0.0;
    
    std::cout << "\n=== Virtual Memory Statistics ===\n";
    std::cout << "Total page accesses: " << total_accesses << "\n";
    std::cout << "Page hits: " << page_hits << "\n";
    std::cout << "Page faults: " << page_faults << "\n";
    std::cout << "Page hit ratio: " << std::fixed << std::setprecision(2) 
              << hit_ratio << "%\n";
    std::cout << "Page fault ratio: " << std::fixed << std::setprecision(2) 
              << (100.0 - hit_ratio) << "%\n";
    std::cout << "=================================\n\n";
}