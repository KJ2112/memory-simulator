# Memory Management Simulator

A comprehensive C++ implementation of an operating system memory management simulator that includes dynamic memory allocation, buddy allocation, multilevel cache simulation, and virtual memory with paging.

Demo video link: https://drive.google.com/file/d/1NEQOFaRo4Jc58WfXoKprLBR0_SiMi3R_/view?usp=sharing
## Features

### 1. Physical Memory Simulation
- Contiguous block memory management
- Dynamic allocation and deallocation
- Block splitting and coalescing
- Fragmentation tracking

### 2. Allocation Strategies
- **First Fit**: Allocates the first sufficiently large block
- **Best Fit**: Allocates the smallest adequate block
- **Worst Fit**: Allocates the largest available block

### 3. Buddy Allocator
- Power-of-two block allocation
- Efficient buddy coalescing
- Recursive splitting and merging
- Minimal external fragmentation

### 4. Multilevel Cache Simulation
- Configurable cache hierarchy (L1, L2, L3)
- Set-associative cache organization
- Replacement policies:
  - FIFO (First In, First Out)
  - LRU (Least Recently Used)
- Hit/miss ratio tracking

### 5. Virtual Memory System
- Page table management
- Address translation
- Page replacement policies:
  - FIFO
  - LRU
- Page fault handling and tracking

## Directory Structure

```
memory-simulator/
├── src/
│   ├── main.cpp                           # Main CLI program
│   ├── allocator/
│   │   └── MemoryManager.cpp              # Standard allocator implementation
│   ├── buddy/
│   │   └── BuddyAllocator.cpp             # Buddy allocator implementation
│   ├── cache/
│   │   └── Cache.cpp                      # Cache simulator implementation
│   └── virtual_memory/
│       └── VirtualMemory.cpp              # Virtual memory implementation
├── include/
│   ├── Block.h                            # Memory block structure
│   ├── MemoryManager.h                    # Memory manager header
│   ├── BuddyAllocator.h                   # Buddy allocator header
│   ├── Cache.h                            # Cache simulator header
│   └── VirtualMemory.h                    # Virtual memory header
├── tests/
│   ├── test_allocator.sh                  # Allocator test script
│   ├── test_buddy.sh                      # Buddy allocator test
│   ├── test_cache.sh                      # Cache test script
│   └── test_vm.sh                         # Virtual memory test
├── docs/
│   └── design_document.md                 # Detailed design documentation
├── Makefile                               # Build configuration
└── README.md                              # This file
```

## Building the Project

### Prerequisites
- C++ compiler with C++17 support (g++ 7.0+ or clang++ 5.0+)
- Make utility

### Compilation

```bash
# Build the project
make

# Build and run
make run

# Clean build files
make clean
```

The executable will be created at `bin/memsim`.

## Usage

### Starting the Simulator

```bash
./bin/memsim
```

### Available Commands

#### Mode Selection
```
mode <standard|buddy|cache|vm>  - Switch between simulator modes
```

#### Standard/Buddy Allocator Commands
```
init memory <size>              - Initialize memory (size in bytes)
set allocator <strategy>        - Set allocation strategy (first_fit, best_fit, worst_fit)
malloc <size>                   - Allocate memory block
free <id>                       - Free allocated block by ID
dump                            - Display memory layout
stats                           - Show allocation statistics
```

#### Cache Simulator Commands
```
init cache <name> <size> <block_size> <assoc> [policy]
                                - Initialize cache
                                  policy: fifo (default) or lru
access <address>                - Access memory address
reset                           - Reset cache statistics
stats                           - Show cache statistics
```

#### Virtual Memory Commands
```
init vm <num_pages> <page_size> <num_frames>
                                - Initialize virtual memory
set policy <fifo|lru>           - Set page replacement policy
translate <virt_addr>           - Translate virtual address
reset                           - Reset VM statistics
stats                           - Show VM statistics
```

#### General Commands
```
help                            - Show help message
exit                            - Exit simulator
```

## Example Usage

### Example 1: Standard Allocator

```
memsim> mode standard
memsim> init memory 1024
memsim> set allocator first_fit
memsim> malloc 100
Allocated block id=1 at address=0x0000
memsim> malloc 200
Allocated block id=2 at address=0x0064
memsim> free 1
Block 1 freed and merged
memsim> dump
[0x0000 - 0x0063] FREE [100 bytes]
[0x0064 - 0x012B] USED (id=2) [200 bytes]
[0x012C - 0x03FF] FREE [724 bytes]
memsim> stats
Total memory: 1024 bytes
Used memory: 200 bytes
External fragmentation: 12%
```

### Example 2: Buddy Allocator

```
memsim> mode buddy
memsim> init memory 1024
memsim> malloc 100
Allocated block id=1 at address=0x0000 (requested: 100, actual: 128 bytes)
memsim> malloc 200
Allocated block id=2 at address=0x0080 (requested: 200, actual: 256 bytes)
memsim> free 1
Block 1 freed and merged
memsim> dump
```

### Example 3: Cache Simulation

```
memsim> mode cache
memsim> init cache L1 1024 64 4 lru
memsim> access 0x1000
Address 0x1000: MISS
memsim> access 0x1000
Address 0x1000: HIT
memsim> access 0x1040
Address 0x1040: HIT
memsim> stats
```

### Example 4: Virtual Memory

```
memsim> mode vm
memsim> init vm 64 256 16
memsim> set policy lru
memsim> translate 0x1000
Page fault: loading page 16 into frame 0
Virtual address 0x1000 -> Physical address 0x0000
memsim> translate 0x1000
Virtual address 0x1000 -> Physical address 0x0000
memsim> stats
```

## Testing

Test scripts are provided in the `tests/` directory:

```bash
# Test standard allocator
./tests/test_allocator.sh

# Test buddy allocator
./tests/test_buddy.sh

# Test cache simulator
./tests/test_cache.sh

# Test virtual memory
./tests/test_vm.sh
```

## Implementation Details

### Memory Allocation
- Uses linked list structure for tracking memory blocks
- Automatic coalescing on deallocation
- Tracks internal and external fragmentation
- Configurable allocation strategies

### Buddy Allocator
- Uses std::map for free lists indexed by block size
- XOR-based buddy address computation
- Recursive splitting and coalescing
- Power-of-two alignment enforcement

### Cache Simulation
- Set-associative cache organization
- Configurable size, block size, and associativity
- LRU uses timestamps for tracking
- FIFO maintains insertion order

### Virtual Memory
- Array-based page table
- Frame allocation tracking
- Multiple page replacement algorithms
- Page fault handling with victim selection

## Performance Considerations

- **Standard Allocator**: O(n) worst case for allocation (n = number of blocks)
- **Buddy Allocator**: O(log n) for allocation and deallocation
- **Cache Access**: O(k) where k is associativity
- **VM Translation**: O(1) for page table lookup

## Limitations

- Simulated memory only (no actual OS integration)
- Single process support in virtual memory
- No disk I/O simulation (symbolic page loading)
- Memory sizes limited by available RAM

## Extensions

Possible extensions for this project:

1. **Multi-process VM**: Support multiple page tables
2. **TLB Simulation**: Add Translation Lookaside Buffer
3. **Clock Replacement**: Implement clock algorithm for paging
4. **Segmentation**: Add segment-based memory management
5. **Graphical Visualization**: Add GUI for memory visualization
6. **Performance Profiling**: Add timing analysis

## References

- Operating System Concepts – Silberschatz, Galvin, Gagne
- Modern Operating Systems – Andrew Tanenbaum
- [GeeksForGeeks - Operating Systems](https://www.geeksforgeeks.org/operating-systems/)

## License

This project is for educational purposes.

## Authors

KJ2112

## Acknowledgments


Based on the OS memory management concepts from standard operating systems textbooks and academic resources.

