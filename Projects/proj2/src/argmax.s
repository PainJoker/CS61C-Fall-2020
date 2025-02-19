.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:
    # Prologue
    li t0, 1
    blt a1, t0, exit_77

    mv t0, zero  # t0 for the counter
    mv t1, zero  # t1 for the return value
    lw t2, 0(a0)   # t2 stores the max value of the vector 

loop_start:
    slli t3, t0, 2
    add t4, a0, t3  # t4 stores &array[i]
    lw t5, 0(t4)  # load the array[i]
    ble t5, t2, loop_continue
    mv t1, t0
    mv t2, t5
loop_continue:
    addi t0, t0, 1
    blt t0, a1, loop_start
loop_end:
    mv a0, t1

    # Epilogue

    ret

exit_77:
    li a1, 77
    j exit2