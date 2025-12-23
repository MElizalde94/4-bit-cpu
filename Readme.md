# 4-Bit CPU Simulator

A simple educational 4-bit CPU simulator written in C++.

## Architecture

### Components
- **4 Registers**: A, B, C, D (each holds 4-bit values: 0-15)
- **16 Bytes RAM**: Addressable memory locations (4-bit addressing)
- **Program Counter (PC)**: Points to current instruction
- **Zero Flag**: Set when arithmetic result is zero
- **4-bit Data Bus**: All values limited to 0-15

### Instruction Format
Each instruction is 8 bits (1 byte):
```
[OPCODE (4 bits)][OPERAND (4 bits)]
```

## Instruction Set

| Opcode | Mnemonic | Description |
|--------|----------|-------------|
| 0x0 | NOP | No operation |
| 0x1 | LDA #n | Load immediate value n into register A |
| 0x2 | LDB #n | Load immediate value n into register B |
| 0x3 | STA [addr] | Store A to memory address |
| 0x4 | STB [addr] | Store B to memory address |
| 0x5 | ADD | Add B to A (result in A) |
| 0x6 | SUB | Subtract B from A (result in A) |
| 0x7 | JMP addr | Jump to address |
| 0x8 | JZ addr | Jump to address if zero flag is set |
| 0x9 | MOV | Move between registers |
| 0xA | LDM [addr] | Load from memory to A |
| 0xB | OUT reg | Output register value |
| 0xC | INC reg | Increment register |
| 0xD | DEC reg | Decrement register |
| 0xE | AND | Logical AND (A & B -> A) |
| 0xF | HLT | Halt execution |

## Example Programs

### 1. Basic Addition (5 + 3)
```cpp
0x15  // LDA #5   - Load 5 into A
0x23  // LDB #3   - Load 3 into B
0x50  // ADD      - Add B to A (A = 8)
0xB0  // OUT A    - Output A
0xF0  // HLT      - Halt
```

### 2. Countdown Loop (5 to 0)
```cpp
0x15  // LDA #5    - Load 5 into A
0xB0  // OUT A     - Output A
0xD0  // DEC A     - Decrement A
0x81  // JZ 1      - Jump to address 1 if zero
0x71  // JMP 1     - Jump back to OUT
```

### 3. Memory Operations
```cpp
0x1A  // LDA #10   - Load 10 into A
0x3F  // STA [15]  - Store A to RAM[15]
0x10  // LDA #0    - Clear A
0xB0  // OUT A     - Output A (0)
0xAF  // LDM [15]  - Load from RAM[15] to A
0xB0  // OUT A     - Output A (10)
0xF0  // HLT       - Halt
```

## Compilation and Running

Using g++:
```bash
g++ -std=c++17 -o cpu4bit cpu4bit.cpp
./cpu4bit
```

Using clang++:
```bash
clang++ -std=c++17 -o cpu4bit cpu4bit.cpp
./cpu4bit
```

## Features

- **Step-by-step execution**: See each instruction execute with debug output
- **Full state inspection**: View registers, PC, flags, and RAM after execution
- **Automatic 4-bit masking**: All values automatically wrapped to 4-bit range
- **Multiple example programs**: Includes arithmetic, loops, and memory operations

## Design Decisions

1. **8-bit instructions with 4-bit components**: While this is a "4-bit CPU" (4-bit data width), 
   instructions are 8 bits to allow for a 4-bit opcode and 4-bit operand. This is similar to 
   real historical 4-bit CPUs.

2. **Von Neumann architecture**: Program and data share the same 16-byte memory space.

3. **Limited instruction set**: Focused on educational clarity rather than completeness.

4. **Register-based operations**: Most operations work with registers for simplicity.

## Extending the CPU

You can easily extend this CPU by:

1. Adding new instructions (update the Opcode enum)
2. Adding more registers
3. Expanding memory size
4. Adding more flags (carry, negative, etc.)
5. Implementing interrupts
6. Adding I/O ports

## Educational Value

This simulator demonstrates:
- Basic CPU architecture concepts
- Fetch-decode-execute cycle
- Register operations
- Memory addressing
- Conditional branching
- Program counter management
- Arithmetic overflow handling (4-bit wrapping)

Perfect for learning computer architecture fundamentals!