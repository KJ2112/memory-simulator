#include "MemoryManager.h"
#include "BuddyAllocator.h"
#include "Cache.h"
#include "VirtualMemory.h"
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

enum SimulatorMode {
    STANDARD_ALLOCATOR,
    BUDDY_ALLOCATOR,
    CACHE_SIM,
    VIRTUAL_MEMORY_SIM
};

void printHelp() {
    std::cout << "\n=== Memory Management Simulator ===\n";
    std::cout << "Available commands:\n\n";
    std::cout << "General:\n";
    std::cout << "  mode <standard|buddy|cache|vm>  - Switch simulator mode\n";
    std::cout << "  help                             - Show this help message\n";
    std::cout << "  exit                             - Exit the simulator\n\n";
    
    std::cout << "Standard/Buddy Allocator:\n";
    std::cout << "  init memory <size>               - Initialize memory\n";
    std::cout << "  set allocator <first_fit|best_fit|worst_fit> - Set allocation strategy\n";
    std::cout << "  malloc <size>                    - Allocate memory\n";
    std::cout << "  free <id>                        - Free allocated block\n";
    std::cout << "  dump                             - Show memory layout\n";
    std::cout << "  stats                            - Show statistics\n\n";
    
    std::cout << "Cache Simulator:\n";
    std::cout << "  init cache <name> <size> <block_size> <assoc> [fifo|lru] - Init cache\n";
    std::cout << "  access <address>                 - Access memory address\n";
    std::cout << "  reset                            - Reset cache statistics\n";
    std::cout << "  stats                            - Show cache statistics\n\n";
    
    std::cout << "Virtual Memory Simulator:\n";
    std::cout << "  init vm <num_pages> <page_size> <num_frames> - Initialize VM\n";
    std::cout << "  set policy <fifo|lru>            - Set page replacement policy\n";
    std::cout << "  translate <virt_addr>            - Translate virtual address\n";
    std::cout << "  reset                            - Reset VM statistics\n";
    std::cout << "  stats                            - Show VM statistics\n";
    std::cout << "====================================\n\n";
}

int main() {
    std::unique_ptr<MemoryManager> memManager;
    std::unique_ptr<BuddyAllocator> buddyAllocator;
    std::unique_ptr<Cache> cache;
    std::unique_ptr<VirtualMemory> vm;
    
    SimulatorMode mode = STANDARD_ALLOCATOR;
    std::string line;
    
    std::cout << "Memory Management Simulator\n";
    std::cout << "Type 'help' for commands\n\n";
    
    while (true) {
        std::cout << "memsim> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        // General commands
        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Exiting simulator...\n";
            break;
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "mode") {
            std::string mode_str;
            iss >> mode_str;
            
            if (mode_str == "standard") {
                mode = STANDARD_ALLOCATOR;
                if (!memManager) {
                    memManager = std::make_unique<MemoryManager>();
                }
                std::cout << "Switched to Standard Allocator mode\n";
            }
            else if (mode_str == "buddy") {
                mode = BUDDY_ALLOCATOR;
                if (!buddyAllocator) {
                    buddyAllocator = std::make_unique<BuddyAllocator>();
                }
                std::cout << "Switched to Buddy Allocator mode\n";
            }
            else if (mode_str == "cache") {
                mode = CACHE_SIM;
                std::cout << "Switched to Cache Simulator mode\n";
            }
            else if (mode_str == "vm") {
                mode = VIRTUAL_MEMORY_SIM;
                std::cout << "Switched to Virtual Memory mode\n";
            }
            else {
                std::cout << "Unknown mode: " << mode_str << "\n";
            }
        }
        // Standard/Buddy allocator commands
        else if (cmd == "init") {
            std::string sub_cmd;
            iss >> sub_cmd;
            
            if (sub_cmd == "memory") {
                size_t size;
                iss >> size;
                
                if (mode == STANDARD_ALLOCATOR) {
                    if (!memManager) {
                        memManager = std::make_unique<MemoryManager>();
                    }
                    memManager->init(size);
                }
                else if (mode == BUDDY_ALLOCATOR) {
                    if (!buddyAllocator) {
                        buddyAllocator = std::make_unique<BuddyAllocator>();
                    }
                    buddyAllocator->init(size);
                }
                else {
                    std::cout << "Error: Not in allocator mode\n";
                }
            }
            else if (sub_cmd == "cache") {
                std::string name;
                size_t size, block_size, assoc;
                std::string policy_str;
                
                iss >> name >> size >> block_size >> assoc;
                
                ReplacementPolicy policy = FIFO;
                if (iss >> policy_str) {
                    if (policy_str == "lru") {
                        policy = LRU;
                    }
                }
                
                cache = std::make_unique<Cache>(name, size, block_size, assoc, policy);
            }
            else if (sub_cmd == "vm") {
                size_t num_pages, page_size, num_frames;
                iss >> num_pages >> page_size >> num_frames;
                
                vm = std::make_unique<VirtualMemory>(num_pages, page_size, num_frames);
            }
            else {
                std::cout << "Unknown init command: " << sub_cmd << "\n";
            }
        }
        else if (cmd == "set") {
            std::string sub_cmd;
            iss >> sub_cmd;
            
            if (sub_cmd == "allocator") {
                std::string strategy_str;
                iss >> strategy_str;
                
                if (mode == STANDARD_ALLOCATOR && memManager) {
                    if (strategy_str == "first_fit") {
                        memManager->setStrategy(FIRST_FIT);
                    }
                    else if (strategy_str == "best_fit") {
                        memManager->setStrategy(BEST_FIT);
                    }
                    else if (strategy_str == "worst_fit") {
                        memManager->setStrategy(WORST_FIT);
                    }
                    else {
                        std::cout << "Unknown strategy: " << strategy_str << "\n";
                    }
                }
                else {
                    std::cout << "Error: Not in standard allocator mode or not initialized\n";
                }
            }
            else if (sub_cmd == "policy") {
                std::string policy_str;
                iss >> policy_str;
                
                if (mode == VIRTUAL_MEMORY_SIM && vm) {
                    if (policy_str == "fifo") {
                        vm->setPolicy(PAGE_FIFO);
                    }
                    else if (policy_str == "lru") {
                        vm->setPolicy(PAGE_LRU);
                    }
                    else {
                        std::cout << "Unknown policy: " << policy_str << "\n";
                    }
                }
                else {
                    std::cout << "Error: Not in VM mode or not initialized\n";
                }
            }
            else {
                std::cout << "Unknown set command: " << sub_cmd << "\n";
            }
        }
        else if (cmd == "malloc") {
            size_t size;
            iss >> size;
            
            if (mode == STANDARD_ALLOCATOR && memManager) {
                memManager->malloc(size);
            }
            else if (mode == BUDDY_ALLOCATOR && buddyAllocator) {
                buddyAllocator->allocate(size);
            }
            else {
                std::cout << "Error: Not in allocator mode or not initialized\n";
            }
        }
        else if (cmd == "free") {
            int id;
            iss >> id;
            
            if (mode == STANDARD_ALLOCATOR && memManager) {
                memManager->free(id);
            }
            else if (mode == BUDDY_ALLOCATOR && buddyAllocator) {
                buddyAllocator->free(id);
            }
            else {
                std::cout << "Error: Not in allocator mode or not initialized\n";
            }
        }
        else if (cmd == "dump") {
            if (mode == STANDARD_ALLOCATOR && memManager) {
                memManager->dump();
            }
            else if (mode == BUDDY_ALLOCATOR && buddyAllocator) {
                buddyAllocator->dump();
            }
            else {
                std::cout << "Error: Not in allocator mode or not initialized\n";
            }
        }
        else if (cmd == "stats") {
            if (mode == STANDARD_ALLOCATOR && memManager) {
                memManager->stats();
            }
            else if (mode == BUDDY_ALLOCATOR && buddyAllocator) {
                buddyAllocator->stats();
            }
            else if (mode == CACHE_SIM && cache) {
                cache->stats();
            }
            else if (mode == VIRTUAL_MEMORY_SIM && vm) {
                vm->stats();
            }
            else {
                std::cout << "Error: Simulator not initialized\n";
            }
        }
        // Cache commands
        else if (cmd == "access") {
            size_t address;
            iss >> address;
            
            if (mode == CACHE_SIM && cache) {
                bool hit = cache->access(address);
                std::cout << "Address 0x" << std::hex << address << std::dec 
                          << ": " << (hit ? "HIT" : "MISS") << "\n";
            }
            else {
                std::cout << "Error: Not in cache mode or not initialized\n";
            }
        }
        else if (cmd == "reset") {
            if (mode == CACHE_SIM && cache) {
                cache->reset();
                std::cout << "Cache statistics reset\n";
            }
            else if (mode == VIRTUAL_MEMORY_SIM && vm) {
                vm->reset();
                std::cout << "Virtual memory statistics reset\n";
            }
            else {
                std::cout << "Error: Reset not available in this mode\n";
            }
        }
        // Virtual memory commands
        else if (cmd == "translate") {
            size_t virt_addr;
            iss >> virt_addr;
            
            if (mode == VIRTUAL_MEMORY_SIM && vm) {
                size_t phys_addr = vm->translate(virt_addr);
                std::cout << "Virtual address 0x" << std::hex << virt_addr 
                          << " -> Physical address 0x" << phys_addr << std::dec << "\n";
            }
            else {
                std::cout << "Error: Not in VM mode or not initialized\n";
            }
        }
        else {
            std::cout << "Unknown command: " << cmd << "\n";
            std::cout << "Type 'help' for available commands\n";
        }
    }
    
    return 0;
}