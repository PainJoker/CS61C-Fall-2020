.data  # declare a section of memory where data variables are stored.
.word 2, 4, 6, 8 # declares an array of words with values 2, 4, 6, and 8 
n: .word 9  # declares a word variable 'n' with the value 9

.text  # declare a section of memeory where code is stored.
main:
    add t0, x0, x0
    addi t1, x0, 1
    la t3, n  # address of n: 0x10000010
    lw t3, 0(t3)
fib:  # calculate fib(n), which is fibonacci number. result = 34
    beq t3, x0, finish
    add t2, t1, t0
    mv t0, t1
    mv t1, t2
    addi t3, t3, -1
    j fib
finish:
    addi a0, x0, 1
    addi a1, t0, 0
    ecall # print integer ecall
    addi a0, x0, 10
    ecall # terminate ecall
