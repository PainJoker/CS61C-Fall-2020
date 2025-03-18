# CS61CPU

Look ma, I made a CPU! Here's what I did:

- Capable of executing more than 30 RISC-V instrcutions
- 5 Phases of execution
    - Instrcution Fetch, IF
    - Instruction Decode, ID
    - Execution, EX
    - Memory Access, MEM
    - Write Back, WB
- Controller specifies how to execute instructions
    - implemented as logic circuit
- Pipelining improves performance
    - 2 stage pipeline separating IF and other phases