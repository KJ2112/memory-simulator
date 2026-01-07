# Memory Management Simulator - Design Document

## 1. Overview

This document describes the design and implementation of a comprehensive memory management simulator that models key operating system memory management subsystems.

## 2. System Architecture

### 2.1 Component Diagram

```
┌─────────────────────────────────────────────────┐
│              Main CLI Interface                  │
│         (Command Parsing & Routing)              │
└─────────┬───────────┬──────────┬────────────────┘
          │           │          │
    ┌─────▼─────┐ ┌──▼────┐ ┌───▼────────┐
    │  Memory   │ │ Cache │ │  Virtual   │
    │  Manager  │ │  Sim  │ │   Memory   │
    └─────┬─────┘ └───────┘ └────────────┘
          │
    ┌─────▼─────────┐
    │    Buddy      │
    │  Allocator    │
    └───────────────┘
```

### 2.2 Module Responsibilities

- **Main CLI**: User interface, command parsing, and mode switching
- **Memory Manager**: Standard allocation algorithms (First/Best/Worst Fit)
- **Buddy Allocator**: Power-of-two buddy allocation system
- **Cache Simulator**: Multilevel cache with replacement policies
- **Virtual Memory**: Paging system with address translation

## 3. Physical Memory Simulation

### 3.1 Memory Layout

Physical memory is represented as a contiguous address space divided into blocks.

```
Memory Layout:
┌──────────┬──────────┬──────────┬──────────┐
│  Block 1 │  Block 2 │  Block 3 │  Block 4 │
│  (FREE)  │  (USED)  │  (FREE)  │  (USED)  │
└──────────┴──────────┴──────────┴──────────┘
```

### 3.2 Data Structures

**Block Structure:**
```cpp
struct Block {
    size_t address;  // Start address
    size_t size;     // Block size
    bool is_free;    // Allocation status
    int id;          // Unique identifier
};
```

**Memory Manager:**
- Maintains `vector<Block>` sorted by address
- Tracks total memory, used memory, and fragmentation
- Implements allocation strategies

### 3.3 Allocation Strategies

#### First Fit
- **Algorithm**: Scan blocks sequentially, allocate first adequate block
- **Time Complexity**: O(n)
- **Advantage**: Fast allocation
- **Disadvantage**: May cause external fragmentation at beginning

#### Best Fit
- **Algorithm**: Find smallest block that fits
- **Time Complexity**: O(n)
- **Advantage**: Minimizes wasted space
- **Disadvantage**: Creates many small unusable holes

#### Worst Fit
- **Algorithm**: Find largest available block
- **Time Complexity**: O(n)
- **Advantage**: Leaves large usable holes
- **Disadvantage**: Quickly exhausts large blocks

### 3.4 Coalescing Algorithm

When a block is freed, adjacent free blocks are merged:

```
Before Free:           After Free & Coalesce:
┌────┬────┬────┐      ┌────────────────┐
│FREE│USED│FREE│  →   │      FREE      │
└────┴────┴────┘      └────────────────┘
```

**Pseudocode:**
```
function coalesce(index):
    // Merge with next block
    while next_block is free:
        merge current with next
        remove next block
    
    // Merge with previous block
    while previous_block is free:
        merge previous with current
        remove current block
```

## 4. Buddy Allocator System

### 4.1 Buddy System Principles

- Memory size must be power of 2
- All allocations rounded to power of 2
- Blocks paired as "buddies" of equal size
- Efficient splitting and coalescing

### 4.2 Block Organization

```
Initial: 1024 bytes
┌──────────────────────────────┐
│         1024 bytes           │
└──────────────────────────────┘

After malloc(100):
┌────┬────┬─────┬──────────────┐
│128 │128 │ 256 │    512       │
│USED│FREE│FREE │    FREE      │
└────┴────┴─────┴──────────────┘
```

### 4.3 Data Structures

```cpp
map<size_t, list<size_t>> free_lists;  // Free lists by size
map<int, BuddyBlock> allocated_blocks;  // Track allocations
```

### 4.4 Buddy Address Calculation

Given a block at address `A` with size `S`:
```
Buddy address = A XOR S
```

Example:
- Block at 0x0000, size 128: Buddy at 0x0080
- Block at 0x0080, size 128: Buddy at 0x0000

### 4.5 Allocation Algorithm

```
function buddyAllocate(size):
    actual_size = nextPowerOfTwo(size)
    
    // Find smallest available block >= actual_size
    block_size = actual_size
    while free_lists[block_size] is empty:
        block_size *= 2
    
    if block_size > total_memory:
        return FAILURE
    
    // Get block and split down to required size
    block = free_lists[block_size].pop()
    while block_size > actual_size:
        block_size /= 2
        buddy = block + block_size
        free_lists[block_size].push(buddy)
    
    return block
```

## 5. Cache Simulation

### 5.1 Cache Organization

```
Set-Associative Cache:

Set 0: [Line 0] [Line 1] [Line 2] [Line 3]  ← 4-way
Set 1: [Line 0] [Line 1] [Line 2] [Line 3]
Set 2: [Line 0] [Line 1] [Line 2] [Line 3]
...
```

### 5.2 Address Mapping

```
Memory Address (32-bit):
┌─────────┬─────────┬─────────┐
│   Tag   │  Index  │ Offset  │
└─────────┴─────────┴─────────┘

Index: Selects cache set
Tag: Identifies specific block
Offset: Byte within block
```

**Calculation:**
```cpp
block_number = address / block_size
set_index = block_number % num_sets
tag = block_number / num_sets
```

### 5.3 Replacement Policies

#### FIFO (First In, First Out)
- Track insertion time
- Evict oldest line on miss
- Simple to implement
- Doesn't consider access patterns

#### LRU (Least Recently Used)
- Update timestamp on each access
- Evict least recently used line
- Better performance
- Reflects temporal locality

### 5.4 Cache Access Algorithm

```
function cacheAccess(address):
    set_index = getSetIndex(address)
    tag = getTag(address)
    
    // Check for hit
    for each line in sets[set_index]:
        if line.valid and line.tag == tag:
            HIT
            if LRU: update line.timestamp
            return
    
    // Miss: find victim and replace
    MISS
    victim = findVictim(set_index)
    sets[set_index][victim].tag = tag
    sets[set_index][victim].valid = true
    sets[set_index][victim].timestamp = current_time
```

## 6. Virtual Memory System

### 6.1 Paging Concepts

- Virtual address space divided into pages
- Physical memory divided into frames
- Page table maps virtual pages to physical frames
- Page size typically 256-4096 bytes

### 6.2 Address Translation

```
Virtual Address:
┌──────────────┬────────────┐
│  Page Number │   Offset   │
└──────────────┴────────────┘
         ↓
    Page Table Lookup
         ↓
Physical Address:
┌──────────────┬────────────┐
│ Frame Number │   Offset   │
└──────────────┴────────────┘
```

**Translation Formula:**
```
page_number = virtual_address / page_size
offset = virtual_address % page_size
frame_number = page_table[page_number].frame
physical_address = frame_number * page_size + offset
```

### 6.3 Page Table Structure

```cpp
struct PageTableEntry {
    bool valid;         // Is page in memory?
    int frame;          // Physical frame number
    size_t timestamp;   // For LRU
};
```

### 6.4 Page Replacement Policies

#### FIFO
- Maintain queue of pages in memory
- Evict oldest page on fault
- Simple but suffers from Belady's anomaly

#### LRU
- Track last access time for each page
- Evict least recently used page
- Better performance, more overhead

### 6.5 Page Fault Handling

```
function handlePageFault(page_num):
    // Try to find free frame
    frame = findFreeFrame()
    
    if frame == -1:
        // No free frames, must evict
        victim = selectVictim()
        frame = page_table[victim].frame
        page_table[victim].valid = false
    
    // Load page into frame
    page_table[page_num].frame = frame
    page_table[page_num].valid = true
    page_table[page_num].timestamp = current_time
```

## 7. Fragmentation Analysis

### 7.1 Internal Fragmentation

**Definition**: Wasted space within allocated blocks

**Calculation**: 
```
internal_frag = Σ(allocated_block_size - requested_size)
```

**Example**:
- Request 100 bytes, allocate 128-byte block
- Internal fragmentation: 28 bytes

### 7.2 External Fragmentation

**Definition**: Free memory scattered in small unusable holes

**Calculation**:
```
external_frag = (total_free - largest_free) / total_free * 100%
```

**Example**:
- Free blocks: 50, 30, 20 bytes (total 100 bytes)
- Largest: 50 bytes
- External fragmentation: (100 - 50) / 100 = 50%

## 8. Performance Metrics

### 8.1 Memory Allocator Metrics

- **Allocation Success Rate**: successful / total allocations
- **Memory Utilization**: used / total memory
- **Average Search Time**: Time to find suitable block
- **Fragmentation Ratio**: Internal + external fragmentation

### 8.2 Cache Metrics

- **Hit Ratio**: hits / (hits + misses)
- **Miss Ratio**: misses / (hits + misses)
- **Average Access Time**: hit_time * hit_ratio + miss_penalty * miss_ratio

### 8.3 Virtual Memory Metrics

- **Page Fault Rate**: page_faults / total_accesses
- **Effective Access Time**: EAT = p * (memory_access_time + page_fault_service_time) + (1-p) * memory_access_time

## 9. Integration Architecture

### 9.1 Complete Memory Access Flow

```
Virtual Address (if VM enabled)
        ↓
   Page Table Lookup
        ↓
  Physical Address
        ↓
   L1 Cache Check
        ↓ (miss)
   L2 Cache Check
        ↓ (miss)
   Main Memory Access
```

### 9.2 Command Processing Pipeline

```
User Input → Parser → Mode Router → Component → Response
```

## 10. Testing Strategy

### 10.1 Unit Tests

- Individual allocation operations
- Coalescing correctness
- Cache hit/miss detection
- Page table updates

### 10.2 Integration Tests

- Multiple allocations/deallocations
- Cache hierarchy behavior
- VM with page replacement
- Fragmentation scenarios

### 10.3 Test Scenarios

1. **Sequential Allocation**: Allocate increasing sizes
2. **Random Allocation**: Random sizes and orders
3. **Stress Test**: Allocate until memory exhausted
4. **Coalescing Test**: Alternating alloc/free
5. **Cache Locality**: Sequential vs random access
6. **Page Thrashing**: More pages than frames

## 11. Assumptions and Limitations

### 11.1 Assumptions

- Single-threaded execution
- No actual OS integration
- Synchronous operations
- Deterministic behavior

### 11.2 Limitations

- No disk I/O simulation
- Single process in VM mode
- No memory-mapped files
- No shared memory
- Limited to simulated address space

## 12. Future Enhancements

### 12.1 Planned Features

- **TLB Simulation**: Translation Lookaside Buffer
- **Multi-level Page Tables**: Hierarchical paging
- **Working Set Tracking**: Monitor page usage
- **Demand Paging**: Load-on-demand simulation

### 12.2 Possible Extensions

- GUI visualization
- Performance profiling
- Comparative analysis tools
- Real-time statistics
- Multi-process support
- Segmentation support

## 13. References

1. Silberschatz, A., Galvin, P. B., & Gagne, G. (2018). Operating System Concepts (10th ed.)
2. Tanenbaum, A. S., & Bos, H. (2014). Modern Operating Systems (4th ed.)
3. Patterson, D. A., & Hennessy, J. L. (2017). Computer Organization and Design (5th ed.)


