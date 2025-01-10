.globl factorial

.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit

factorial:  # Assume n > 0
    addi t0, zero, 1  # condition check
    mv t1, a0  # iterative variable
    addi a0, zero, 1  # result
loop:
    bne t1, t0, multiply
    ret  # result = 1
multiply:
    mul a0, a0, t1
    addi t1, t1, -1
    j loop