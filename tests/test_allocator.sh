#!/bin/bash
cd "$(dirname "$0")"

# Test script for Standard Memory Allocator

echo "=== Testing Standard Memory Allocator ==="
echo ""

# Create test input
cat > test_allocator_input.txt << EOF
mode standard
init memory 1024
set allocator first_fit
malloc 100
malloc 200
malloc 150
dump
stats
free 2
dump
malloc 50
dump
stats
set allocator best_fit
malloc 100
dump
stats
set allocator worst_fit
malloc 80
dump
stats
exit
EOF

echo "Running test with first_fit, best_fit, and worst_fit..."
../bin/memsim.exe < test_allocator_input.txt > ../test_allocator_output.txt

echo ""
echo "Test complete. Output saved to test_allocator_output.txt"
echo ""
echo "=== Output Preview ==="
head -50 test_allocator_output.txt

# Cleanup
rm test_allocator_input.txt