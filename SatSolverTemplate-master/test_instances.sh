#!/bin/bash
# filepath: /home/kingmrock/tp_insa/SatSolverTemplate-master/test_instances.sh

# Default max difficulty if not specified
DEFAULT_DIFFICULTY="challenging"

# Array of difficulties in order
DIFFICULTIES=("trivial" "easy" "medium" "hard" "challenging" "impossible")

# Function to check if first difficulty is less than or equal to second
# Returns 0 if less than or equal, 1 otherwise
difficulty_le() {
    local diff1=$1
    local diff2=$2
    local found1=false
    local found2=false
    
    for d in "${DIFFICULTIES[@]}"; do
        if [[ $d == $diff1 ]]; then
            found1=true
        fi
        if [[ $d == $diff2 ]]; then
            found2=true
            if [[ $found1 == true ]]; then
                return 0
            fi
        fi
    done
    return 1
}

# Get max difficulty from command line or use default
MAX_DIFFICULTY=${1:-$DEFAULT_DIFFICULTY}

# Validate max difficulty
valid_difficulty=false
for d in "${DIFFICULTIES[@]}"; do
    if [[ $d == $MAX_DIFFICULTY ]]; then
        valid_difficulty=true
        break
    fi
done

if [[ $valid_difficulty == false ]]; then
    echo "Invalid difficulty: $MAX_DIFFICULTY"
    echo "Valid difficulties: ${DIFFICULTIES[*]}"
    exit 1
fi

# Counter for statistics
total_tests=0
passed_tests=0

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

for type in sat unsat; do
    for difficulty in "${DIFFICULTIES[@]}"; do
        # Stop if we've reached max difficulty
        difficulty_le "$difficulty" "$MAX_DIFFICULTY" || break
        
        # Process all CNF files in this directory
        for file in eval/$type/$difficulty/*.cnf; do
            if [[ -f $file ]]; then
                echo -n "Testing $file: "
                total_tests=$((total_tests + 1))
                
                # Run the solver and capture output
                output=$(./build/solve "$file")
                first_line=$(echo "$output" | head -n 1)
                
                # Check result based on instance type
                if [[ $type == "unsat" ]]; then
                    if [[ $first_line == "UNSAT" ]]; then
                        echo -e "${GREEN}PASS${NC}"
                        passed_tests=$((passed_tests + 1))
                    else
                        echo -e "${RED}FAIL${NC} (Expected UNSAT)"
                    fi
                else
                    if [[ $first_line == "UNSAT" ]]; then
                        echo -e "${RED}FAIL${NC} (Got UNSAT for SAT instance)"
                    elif [[ $first_line =~ ^p\ cnf ]]; then
                        echo -e "${GREEN}PASS${NC}"
                        passed_tests=$((passed_tests + 1))
                    else
                        echo -e "${RED}FAIL${NC} (Invalid output format)"
                    fi
                fi
            fi
        done
    done
done


# Print summary
echo "----------------------------------------"
echo "Summary: $passed_tests/$total_tests tests passed"
if [[ $passed_tests == $total_tests ]]; then
    echo -e "${GREEN}All tests passed!${NC}"
else
    echo -e "${RED}Some tests failed${NC}"
fi