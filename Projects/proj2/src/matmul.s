.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:

    # Error checks
    li t0, 1
    blt a1, t0, exit_72
    blt a2, t0, exit_72
    blt a4, t0, exit_73
    blt a5, t0, exit_73
    bne a2, a4, exit_74


    # Prologue
    addi sp, sp, -40
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw s6, 24(sp)
    sw s7, 28(sp)
    sw s8, 32(sp)
    sw ra, 36(sp)

    mv s0, a0  # s0 is the pointer to the start of m0
    mv s1, a1  # s1 is # of rows of m0
    mv s2, a2  # s2 is # of cols of m0
    mv s3, a3  # s3 is the pointer to the start of m1
    mv s4, a4  # s4 is # of rows of m1
    mv s5, a5  # s5 is # of cols of m1
    mv s6, a6  # s6 is the pointer to the start of d 
    mv s7, zero  # s7 for outer counter

outer_loop_start:
    mv s8, zero  # s8 for the inner counter
inner_loop_start:
    mul t0, s7, s2  # the # of m0[i]
    slli t0, t0, 2  # word offset
    add a0, s0, t0  # addr of s7-th row of m0
    mv a2, s2  # vector length
    li a3, 1  # stride of m0

    slli t0, s8, 2  # word offset for m1
    add a1, s3, t0  # addr of s8-th col of m1
    mv a4, s5  # stride of m1
    jal dot  # exec the dot, a0 stores the result

    mul t0, s7, s5
    add t0, t0, s8   # element offset for d
    slli t0, t0, 2  # word offset
    add t1, t0, s6  # addr of d[s7, s8]
    sw a0, 0(t1)

    addi s8, s8, 1
    blt s8, s5, inner_loop_start
inner_loop_end:
    addi s7, s7, 1
    blt s7, s1, outer_loop_start
outer_loop_end:


    # Epilogue
    lw ra, 36(sp)
    lw s8, 32(sp)
    lw s7, 28(sp)
    lw s6, 24(sp)
    lw s5, 20(sp)
    lw s4, 16(sp)
    lw s3, 12(sp)
    lw s2, 8(sp)
    lw s1, 4(sp)
    lw s0, 0(sp)
    addi sp, sp, 40
    
    
    ret

exit_72:
    li a1, 72
    j exit_common
exit_73:
    li a1, 73
    j exit_common
exit_74:
    li a1, 74
    j exit_common

exit_common:
    j exit2