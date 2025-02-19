.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # Prologue
    li t0, 1
    blt a1, t0, exit_78

    add t0, t0, zero  # t0 for the counter

loop_start:
    slli t3, t0, 2
    add t1, a0, t3  # t1 store the addr of element
    lw t2, 0(t1)  # t2 store the element
    bge t2, zero, loop_continue
    sw zero, 0(t1)
loop_continue:
    addi t0, t0, 1
    blt t0, a1, loop_start
loop_end:


    # Epilogue

	ret

exit_78:
    li a1, 78
    j exit2