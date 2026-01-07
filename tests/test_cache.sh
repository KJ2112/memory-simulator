#!/bin/bash
cd "$(dirname "$0")"

# Test script for Cache Simulator

echo "=== Testing Cache Simulator ==="
echo ""

# Create test input
cat > test_cache_input.txt << EOF
mode cache
init cache L1 1024 64 4 lru
access 0
access 64
access 128
access 192
access 0
access 64
access 256
access 320
access 384
access 448
access 0
stats
reset
access 1000
access 2000
access 3000
access 1000
stats
exit
EOF

echo "Running cache simulation test..."
../bin/memsim.exe < test_cache_input.txt > ../test_cache_output.txt

echo ""
echo "Test complete. Output saved to test_cache_output.txt"
echo ""
echo "=== Output Preview ==="
cat test_cache_output.txt

# Cleanup
rm test_cache_input.txt