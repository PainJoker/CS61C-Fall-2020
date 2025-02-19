.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue
    addi sp, sp, -28
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)

    mv s0, a0
    mv s1, a1
    mv s2, a2

    # open file with read permission
    mv a1, s0
    mv a2, zero
    jal fopen
    blt a0, zero, exit_90
    mv s3, a0  # s3 stores the file discriptor

    # laod the first 8 bytes of the file, which are rows and cols
    mv a1, s3
    mv a2, s1
    li a3, 4
    jal fread
    li t0, 4
    bne a0, t0, exit_91

    mv a1, s3
    mv a2, s2
    li a3, 4
    jal fread
    li t0, 4
    bne a0, t0, exit_91

    # memory allocate to matrix buffer
    lw t0, 0(s1)
    lw t1, 0(s2)
    mul s4, t0, t1  # s4 stores the # of elements in the matrix
    slli a0, s4, 2
    jal malloc
    beq a0, zero, exit_88
    mv s5, a0  # s5 stores the address of read matrix

    # read the matrix
    mv a1, s3
    mv a2, s5
    slli a3, s4, 2
    jal fread
    slli t0, s4, 2
    bne a0, t0, exit_91

    # close the file
    mv a0, s3
    jal fclose
    blt a0, zero, exit_92

    # prepare the return value
    mv a0, s5

    # Epilogue
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 28


    ret

exit_88:
    li a1, 88
    j exit_common

exit_90:
    li a1, 90
    j exit_common

exit_91:
    li a1, 91
    j exit_common

exit_92:
    li a1, 92
    j exit_common

exit_common:
    j exit2