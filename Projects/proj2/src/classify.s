.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    # args # check
    li t0, 5
    bne a0, t0, exit_89

    # prologue
    addi sp, sp, -52
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10, 44(sp)
    sw s11, 48(sp)

    # the content in a1 is the char* (i.e., base addr of argv.)
    mv t0, a1  
    mv s11, a2
    lw s0, 4(t0)  # s0 stores m0 path
    lw s1, 8(t0)  # s1 stores m1 path
    lw s2, 12(t0)  # s2 stores input path
    lw s3, 16(t0)  # s3 stores output path

	# =====================================
    # LOAD MATRICES
    # =====================================
    # Load pretrained m0
    li a0, 8
    jal malloc
    beq a0, zero, exit_88
    mv s4, a0  # s4 stores the &row of m0, s4 + 4 = &col

    mv a0, s0
    mv a1, s4
    addi a2, a1, 4
    jal read_matrix
    beq a0, zero, exit_88
    mv s0, a0  # s0 stores the addr of m0

    # Load pretrained m1
    li a0, 8
    jal malloc
    beq a0, zero, exit_88
    mv s5, a0  # s5 stores the &row of m1, s5 + 4 = &col

    mv a0, s1
    mv a1, s5
    addi a2, a1, 4
    jal read_matrix
    beq a0, zero, exit_88
    mv s1, a0  # s1 stores the addr of m1

    # Load input matrix
    li a0, 8
    jal malloc
    beq a0, zero, exit_88
    mv s6, a0  # s6 stores the &row of input, s6 + 4 = &col

    mv a0, s2
    mv a1, s6
    addi a2, a1, 4
    jal read_matrix
    beq a0, zero, exit_88
    mv s2, a0  # s2 stores the addr of input matrix
    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)

    # malloc the FIRST LINEAR LAYER
    lw t0, 0(s4)
    lw s9, 4(s6)  # s9 stores the # cols of input
    mul s8, t0, s9  # s8 stores the # of element of LINEAR LAYER
    slli a0, s8, 2
    jal malloc
    beq a0, zero, exit_88
    mv s7, a0  # s7 stores addr m0 * input

    # m0 * input
    mv a0, s0
    lw a1, 0(s4)
    lw a2, 4(s4)
    mv a3, s2
    lw a4, 0(s6)
    mv a5, s9
    mv a6, s7
    jal matmul
    
    # NONLINEAR LAYER
    mv a0, s7
    mv a1, s8 
    jal relu

    # malloc the m1 * ReLU(m0 * input)
    lw t0, 0(s5) 
    mul s8, t0, s9  # overwtites the s8 to prepare maxarg
    slli a0, s8, 2
    jal malloc
    beq a0, zero, exit_88
    mv s10, a0

    # LINEAR LAYER m1 * ReLU(m0 * input)
    mv a0, s1
    lw a1, 0(s5)
    lw a2, 4(s5)
    mv a3, s7
    lw a4, 0(s4)
    mv a5, s9
    mv a6, s10
    jal matmul
    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix
    mv a0, s3
    mv a1, s10
    lw a2, 0(s5)
    mv a3, s9
    jal write_matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax
    mv a0, s10
    mv a1, s8
    jal argmax
    mv s8, a0  # overwrites s8 to output

    # Print classification
    bne s11, zero, done
    mv a1, s8
    jal print_int
    # Print newline afterwards for clarity
    li a1, '\n'
    jal print_char

done:
    mv a0, s0
    jal free
    mv a0, s1
    jal free
    mv a0, s2
    jal free

    mv a0, s4
    jal free
    mv a0, s5
    jal free
    mv a0, s6
    jal free

    mv a0, s7
    jal free
    mv a0, s10
    jal free
    # epilogue
    lw s11, 48(sp)
    lw s10, 44(sp)
    lw s9, 40(sp)
    lw s8, 36(sp)
    lw s7, 32(sp)
    lw s6, 28(sp)
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 52

    ret

exit_89:
    li a1, 89
    j exit_common

exit_88:
    li a1, 88
    j exit_common

exit_common:
    j exit2