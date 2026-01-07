#!/bin/bash

# Test script for Virtual Memory Simulator

echo "=== Testing Virtual Memory Simulator ==="
echo ""

# Create test input
cat > test_vm_input.txt << EOF
mode vm
init vm 64 256 8
set policy fifo
translate 0x0000
translate 0x0100
translate 0x0200
translate 0x0300
translate 0x0400
translate 0x0500
translate 0x0600
translate 0x0700
translate 0x0800
translate 0x0000
translate 0x0100
stats
reset
set policy lru
translate 0x1000
translate 0x2000
translate 0x3000
translate 0x1000
translate 0x4000
translate 0x1000
stats
exit
EOF

echo "Running virtual memory simulation test..."
../bin/memsim < test_vm_input.txt > test_vm_output.txt

echo ""
echo "Test complete. Output saved to test_vm_output.txt"
echo ""
echo "=== Output Preview ==="
cat test_vm_output.txt

# Cleanup
rm test_vm_input.txt