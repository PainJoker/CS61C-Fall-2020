#!/bin/bash

# Check if the user provided a directory path
if [ -z "$1" ]; then
  echo "Usage: $0 <function to be tested>"
  exit 1
fi

# Navigate to the specified directory
cd tests/part_b/custom/ || { echo "Not in root directory."; exit 1; }

# Convert binary reference output to hex
python3 binary_to_hex_cpu.py reference_output/cpu-"$1"-ref.out > reference.out || { echo "Failed to convert reference output"; exit 1; }

# Convert binary student output to hex
python3 binary_to_hex_cpu.py student_output/cpu-"$1"-student.out > student.out || { echo "Failed to convert student output"; exit 1; }

# Compare the two hex files
diff reference.out student.out > diff.txt

# clean the .out file
# rm student.out
# rm reference.out
