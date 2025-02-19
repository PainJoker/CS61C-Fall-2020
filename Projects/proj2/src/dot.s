.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:
    # Prologue

    li t0, 1
    blt a2, t0, exit_75
    blt a3, t0, exit_76
    blt a4, t0, exit_76

    mv t0, zero  # t0 for the counter
    mv t1, zero  # t1 store the result
loop_start:
    slli t2, t0, 2
    mul t3, t2, a3  # t3 stores the &v0[i * stride]
    add t3, a0, t3
    mul t4, t2, a4  # t4 stores the &v1[i * stride]
    add t4, a1, t4
    lw t5, 0(t3)  # t5 stores v0[i]
    lw t6, 0(t4)  # t6 stores v1[i]
    mul t2, t5, t6  # t2 stores multiplication result
    add t1, t1, t2
    addi t0, t0, 1
    blt t0, a2, loop_start
loop_end:
    mv a0, t1

    # Epilogue

    ret

exit_75:
    li a1 75
    j exit_common
exit_76:
    li a1 76
    j exit_common

exit_common:
    j exit2