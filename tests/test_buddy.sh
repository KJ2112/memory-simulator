#!/bin/bash
cd "$(dirname "$0")"

# Test script for Buddy Allocator

echo "=== Testing Buddy Allocator ==="
echo ""

# Create test input
cat > test_buddy_input.txt << EOF
mode buddy
init memory 1024
malloc 50
malloc 100
malloc 200
malloc 75
dump
stats
free 2
dump
free 1
dump
malloc 150
dump
stats
exit
EOF

echo "Running buddy allocator test..."
../bin/memsim.exe < test_buddy_input.txt > ../test_buddy_output.txt

echo ""
echo "Test complete. Output saved to test_buddy_output.txt"
echo ""
echo "=== Output Preview ==="
head -50 test_buddy_output.txt

# Cleanup
rm test_buddy_input.txt