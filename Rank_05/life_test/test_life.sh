#!/bin/bash

# Game of Life Test Script
# Compares outputs between life-d (deniz) and life-m (mari) implementations

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counter
test_count=0
passed_tests=0
failed_tests=0

# Function to run a single test
run_test() {
    local test_name="$1"
    local input="$2"
    local width="$3"
    local height="$4"
    local iterations="$5"
    
    test_count=$((test_count + 1))
    
    echo -e "${YELLOW}Running Test $test_count: $test_name${NC}"
    echo "Input: '$input'"
    echo "Command: ./life $width $height $iterations"
    echo "----------------------------------------"
    
    # Run deniz solution
    deniz_output=$(echo "$input" | ./life-d "$width" "$height" "$iterations" 2>&1)
    deniz_exit_code=$?
    
    # Run mari solution  
    mari_output=$(echo "$input" | ./life-m "$width" "$height" "$iterations" 2>&1)
    mari_exit_code=$?
    
    # Save outputs to files
    echo "$deniz_output" > "test${test_count}_deniz_output.txt"
    echo "$mari_output" > "test${test_count}_mari_output.txt"
    
    # Compare outputs
    if [ "$deniz_output" = "$mari_output" ] && [ "$deniz_exit_code" = "$mari_exit_code" ]; then
        echo -e "${GREEN}✓ PASS: Outputs match${NC}"
        passed_tests=$((passed_tests + 1))
    else
        echo -e "${RED}✗ FAIL: Outputs differ${NC}"
        failed_tests=$((failed_tests + 1))
        
        echo "Deniz output (exit code: $deniz_exit_code):"
        echo "$deniz_output" | cat -e
        echo
        echo "Mari output (exit code: $mari_exit_code):"
        echo "$mari_output" | cat -e
        echo
        
        # Show detailed diff
        echo "Detailed difference:"
        diff -u "test${test_count}_deniz_output.txt" "test${test_count}_mari_output.txt" || true
    fi
    
    echo "----------------------------------------"
    echo
}

# Check if executables exist
if [ ! -f "./life-d" ]; then
    echo -e "${RED}Error: life-d executable not found${NC}"
    exit 1
fi

if [ ! -f "./life-m" ]; then
    echo -e "${RED}Error: life-m executable not found${NC}"
    exit 1
fi

# Make executables runnable
chmod +x ./life-d ./life-m

echo "=== Game of Life Test Suite ==="
echo "Comparing life-d (Deniz) vs life-m (Mari)"
echo "==========================================="
echo

# Test 1: Basic rectangle pattern (0 iterations)
run_test "Basic Rectangle Pattern" "sdxddssaaww" 5 5 0

# Test 2: Complex pattern (0 iterations)  
run_test "Complex Pattern" "sdxssdswdxdddxsaddawxwdxwaa" 10 6 0

# Test 3: Vertical line evolves to horizontal (1 iteration)
run_test "Vertical to Horizontal Line" "dxss" 3 3 1

# Test 4: Evolution cycle (2 iterations - should return to original)
run_test "Evolution Cycle" "dxss" 3 3 2

# Test 5: Glider pattern (moves diagonally - famous Conway pattern)
run_test "Glider Pattern" "ddsxsasx" 6 6 4

# Summary
echo "==========================================="
echo "TEST SUMMARY:"
echo -e "Total tests: $test_count"
echo -e "${GREEN}Passed: $passed_tests${NC}"
echo -e "${RED}Failed: $failed_tests${NC}"

if [ $failed_tests -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests passed! Both implementations are equivalent.${NC}"
    exit 0
else
    echo -e "${RED}⚠️  Some tests failed. Check the differences above.${NC}"
    exit 1
fi
