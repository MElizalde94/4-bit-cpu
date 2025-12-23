#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>
#include <bitset>

class CPU4Bit {
private:
    // 4-bit registers (0-15)
    uint8_t regA;
    uint8_t regB;
    uint8_t regC;
    uint8_t regD;
    
    // Program counter (can address 16 memory locations)
    uint8_t PC;
    
    // Zero flag for conditional operations
    bool zeroFlag;
    
    // 16 bytes of RAM (4-bit addresses)
    uint8_t RAM[16];
    
    // Running state
    bool running;
    
    // Mask to ensure 4-bit values
    const uint8_t MASK_4BIT = 0x0F;
    
    // Helper function to mask values to 4 bits
    uint8_t mask4bit(uint8_t value) {
        return value & MASK_4BIT;
    }
    
    // Get register by number
    uint8_t& getRegister(uint8_t regNum) {
        switch(regNum & 0x03) {
            case 0: return regA;
            case 1: return regB;
            case 2: return regC;
            case 3: return regD;
            default: return regA; // Should never happen
        }
    }
    
    std::string getRegisterName(uint8_t regNum) {
        switch(regNum & 0x03) {
            case 0: return "A";
            case 1: return "B";
            case 2: return "C";
            case 3: return "D";
            default: return "?";
        }
    }

public:
    // Instruction opcodes (4-bit)
    enum Opcode {
        NOP   = 0x0,  // No operation
        LDA   = 0x1,  // Load immediate to A
        LDB   = 0x2,  // Load immediate to B
        STA   = 0x3,  // Store A to memory
        STB   = 0x4,  // Store B to memory
        ADD   = 0x5,  // Add B to A (result in A)
        SUB   = 0x6,  // Subtract B from A (result in A)
        JMP   = 0x7,  // Jump to address
        JZ    = 0x8,  // Jump if zero flag set
        MOV   = 0x9,  // Move between registers
        LDM   = 0xA,  // Load from memory to A
        OUT   = 0xB,  // Output register value
        INC   = 0xC,  // Increment register
        DEC   = 0xD,  // Decrement register
        ALU   = 0xE,  // Extended ALU operations (uses operand for sub-opcode)
        HLT   = 0xF   // Halt
    };
    
    // ALU sub-opcodes (used when opcode = 0xE)
    enum ALUOp {
        AND_OP = 0x0,  // A & B -> A
        OR_OP  = 0x1,  // A | B -> A
        XOR_OP = 0x2,  // A ^ B -> A
        NOT_OP = 0x3,  // ~A -> A
        SHL_OP = 0x4,  // A << 1 -> A (shift left)
        SHR_OP = 0x5,  // A >> 1 -> A (shift right)
        ROL_OP = 0x6,  // Rotate A left
        ROR_OP = 0x7   // Rotate A right
    };
    
    CPU4Bit() {
        reset();
    }
    
    void reset() {
        regA = 0;
        regB = 0;
        regC = 0;
        regD = 0;
        PC = 0;
        zeroFlag = false;
        running = true;
        
        // Clear RAM
        for(int i = 0; i < 16; i++) {
            RAM[i] = 0;
        }
    }
    
    // Load program into RAM
    void loadProgram(const std::vector<uint8_t>& program) {
        for(size_t i = 0; i < program.size() && i < 16; i++) {
            RAM[i] = program[i];
        }
    }
    
    // Execute one instruction
    void step() {
        if(!running) return;
        
        // Fetch instruction
        uint8_t instruction = RAM[PC];
        uint8_t opcode = (instruction >> 4) & MASK_4BIT;
        uint8_t operand = instruction & MASK_4BIT;
        
        std::cout << "PC=" << std::hex << std::setw(1) << (int)PC 
                  << " Instr=0x" << std::setw(2) << std::setfill('0') << (int)instruction
                  << " Op=0x" << (int)opcode << " Operand=0x" << (int)operand
                  << std::setfill(' ') << std::dec;
        
        PC = mask4bit(PC + 1); // Increment PC
        
        // Decode and execute
        switch(opcode) {
            case NOP:
                std::cout << " NOP" << std::endl;
                break;
                
            case LDA:
                regA = mask4bit(operand);
                std::cout << " LDA #" << (int)operand << " -> A=" << (int)regA << std::endl;
                break;
                
            case LDB:
                regB = mask4bit(operand);
                std::cout << " LDB #" << (int)operand << " -> B=" << (int)regB << std::endl;
                break;
                
            case STA:
                RAM[operand] = regA;
                std::cout << " STA [" << (int)operand << "] <- A=" << (int)regA << std::endl;
                break;
                
            case STB:
                RAM[operand] = regB;
                std::cout << " STB [" << (int)operand << "] <- B=" << (int)regB << std::endl;
                break;
                
            case ADD:
                regA = mask4bit(regA + regB);
                zeroFlag = (regA == 0);
                std::cout << " ADD A+B -> A=" << (int)regA << " Z=" << zeroFlag << std::endl;
                break;
                
            case SUB:
                regA = mask4bit(regA - regB);
                zeroFlag = (regA == 0);
                std::cout << " SUB A-B -> A=" << (int)regA << " Z=" << zeroFlag << std::endl;
                break;
                
            case JMP:
                PC = mask4bit(operand);
                std::cout << " JMP -> PC=" << (int)PC << std::endl;
                break;
                
            case JZ:
                if(zeroFlag) {
                    PC = mask4bit(operand);
                    std::cout << " JZ (taken) -> PC=" << (int)PC << std::endl;
                } else {
                    std::cout << " JZ (not taken)" << std::endl;
                }
                break;
                
            case MOV: {
                uint8_t src = (operand >> 2) & 0x03;
                uint8_t dst = operand & 0x03;
                uint8_t value = getRegister(src);
                getRegister(dst) = value;
                std::cout << " MOV " << getRegisterName(src) << "->" << getRegisterName(dst) 
                         << " (value=" << (int)value << ")" << std::endl;
                break;
            }
                
            case LDM:
                regA = RAM[operand];
                std::cout << " LDM [" << (int)operand << "] -> A=" << (int)regA << std::endl;
                break;
                
            case OUT:
                std::cout << " OUT " << getRegisterName(operand & 0x03) 
                         << "=" << (int)getRegister(operand & 0x03) << " ***" << std::endl;
                break;
                
            case INC:
                getRegister(operand & 0x03) = mask4bit(getRegister(operand & 0x03) + 1);
                zeroFlag = (getRegister(operand & 0x03) == 0);
                std::cout << " INC " << getRegisterName(operand & 0x03) 
                         << "=" << (int)getRegister(operand & 0x03) << std::endl;
                break;
                
            case DEC:
                getRegister(operand & 0x03) = mask4bit(getRegister(operand & 0x03) - 1);
                zeroFlag = (getRegister(operand & 0x03) == 0);
                std::cout << " DEC " << getRegisterName(operand & 0x03) 
                         << "=" << (int)getRegister(operand & 0x03) << std::endl;
                break;
                
            case ALU: {
                // Extended ALU operations using operand as sub-opcode
                switch(operand & 0x0F) {
                    case AND_OP:
                        regA = mask4bit(regA & regB);
                        zeroFlag = (regA == 0);
                        std::cout << " AND A&B -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                        
                    case OR_OP:
                        regA = mask4bit(regA | regB);
                        zeroFlag = (regA == 0);
                        std::cout << " OR A|B -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                        
                    case XOR_OP:
                        regA = mask4bit(regA ^ regB);
                        zeroFlag = (regA == 0);
                        std::cout << " XOR A^B -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                        
                    case NOT_OP:
                        regA = mask4bit(~regA);
                        zeroFlag = (regA == 0);
                        std::cout << " NOT ~A -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                        
                    case SHL_OP:
                        regA = mask4bit(regA << 1);
                        zeroFlag = (regA == 0);
                        std::cout << " SHL A<<1 -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                        
                    case SHR_OP:
                        regA = mask4bit(regA >> 1);
                        zeroFlag = (regA == 0);
                        std::cout << " SHR A>>1 -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                        
                    case ROL_OP: {
                        // Rotate left: shift left and wrap MSB to LSB
                        uint8_t msb = (regA & 0x08) >> 3;  // Get bit 3
                        regA = mask4bit((regA << 1) | msb);
                        zeroFlag = (regA == 0);
                        std::cout << " ROL rotate left -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                    }
                        
                    case ROR_OP: {
                        // Rotate right: shift right and wrap LSB to MSB
                        uint8_t lsb = (regA & 0x01) << 3;  // Get bit 0, move to bit 3
                        regA = mask4bit((regA >> 1) | lsb);
                        zeroFlag = (regA == 0);
                        std::cout << " ROR rotate right -> A=" << (int)regA << " (0b" 
                                 << std::bitset<4>(regA) << ")" << std::endl;
                        break;
                    }
                        
                    default:
                        std::cout << " UNKNOWN ALU OP: 0x" << std::hex << (int)operand 
                                 << std::dec << std::endl;
                        break;
                }
                break;
            }
                
            case HLT:
                running = false;
                std::cout << " HLT - CPU Halted" << std::endl;
                break;
                
            default:
                std::cout << " UNKNOWN OPCODE!" << std::endl;
                break;
        }
    }
    
    // Run until halt
    void run(int maxSteps = 100) {
        int steps = 0;
        while(running && steps < maxSteps) {
            step();
            steps++;
        }
        if(steps >= maxSteps) {
            std::cout << "Max steps reached!" << std::endl;
        }
    }
    
    void printState() {
        std::cout << "\n=== CPU State ===" << std::endl;
        std::cout << "A=" << (int)regA << " B=" << (int)regB 
                  << " C=" << (int)regC << " D=" << (int)regD << std::endl;
        std::cout << "PC=" << (int)PC << " Zero=" << zeroFlag 
                  << " Running=" << running << std::endl;
        
        std::cout << "\n=== RAM ===" << std::endl;
        for(int i = 0; i < 16; i++) {
            std::cout << std::hex << std::setw(1) << i << ":0x" 
                     << std::setw(2) << std::setfill('0') << (int)RAM[i] << " ";
            if((i + 1) % 8 == 0) std::cout << std::endl;
        }
        std::cout << std::dec << std::setfill(' ') << std::endl;
    }
};

int main() {
    CPU4Bit cpu;
    
    std::cout << "===== 4-Bit CPU Simulator =====" << std::endl;
    std::cout << "\n=== Example 1: Basic Addition ===" << std::endl;
    
    // Program: Add 5 + 3 and output result
    std::vector<uint8_t> program1 = {
        0x15,  // LDA #5   - Load 5 into A
        0x23,  // LDB #3   - Load 3 into B
        0x50,  // ADD      - Add B to A
        0xB0,  // OUT A    - Output A (should be 8)
        0xF0   // HLT      - Halt
    };
    
    cpu.loadProgram(program1);
    cpu.run();
    cpu.printState();
    
    std::cout << "\n=== Example 2: Countdown Loop ===" << std::endl;
    cpu.reset();
    
    // Program: Count down from 5 to 0
    std::vector<uint8_t> program2 = {
        0x15,  // 0: LDA #5    - Load 5 into A
        0xB0,  // 1: OUT A     - Output A
        0xD0,  // 2: DEC A     - Decrement A
        0x81,  // 3: JZ 1      - Jump to address 1 (OUT) if zero
        0x71,  // 4: JMP 1     - Jump back to OUT
        0xF0   // 5: HLT       - Halt (unreachable in this program)
    };
    
    cpu.loadProgram(program2);
    cpu.run(20);  // Limit steps to prevent infinite loop
    cpu.printState();
    
    std::cout << "\n=== Example 3: Memory Operations ===" << std::endl;
    cpu.reset();
    
    // Program: Store and load from memory
    std::vector<uint8_t> program3 = {
        0x1A,  // 0: LDA #10   - Load 10 into A
        0x3F,  // 1: STA [15]  - Store A to RAM[15]
        0x10,  // 2: LDA #0    - Clear A
        0xB0,  // 3: OUT A     - Output A (0)
        0xAF,  // 4: LDM [15]  - Load from RAM[15] to A
        0xB0,  // 5: OUT A     - Output A (10)
        0xF0   // 6: HLT       - Halt
    };
    
    cpu.loadProgram(program3);
    cpu.run();
    cpu.printState();
    
    std::cout << "\n=== Example 4: Bitwise Operations (AND, OR, XOR) ===" << std::endl;
    cpu.reset();
    
    // Program: Demonstrate bitwise operations
    // A=1100 (12), B=1010 (10)
    std::vector<uint8_t> program4 = {
        0x1C,  // 0: LDA #12   - Load 12 (0b1100) into A
        0x2A,  // 1: LDB #10   - Load 10 (0b1010) into B
        0xB0,  // 2: OUT A     - Output A=12
        0xB1,  // 3: OUT B     - Output B=10
        0xE0,  // 4: AND       - A & B = 0b1000 (8)
        0xB0,  // 5: OUT A     - Output result
        0x1C,  // 6: LDA #12   - Reload A
        0xE1,  // 7: OR        - A | B = 0b1110 (14)
        0xB0,  // 8: OUT A     - Output result
        0x1C,  // 9: LDA #12   - Reload A
        0xE2,  // A: XOR       - A ^ B = 0b0110 (6)
        0xB0,  // B: OUT A     - Output result
        0xF0   // C: HLT       - Halt
    };
    
    cpu.loadProgram(program4);
    cpu.run();
    cpu.printState();
    
    std::cout << "\n=== Example 5: NOT Operation ===" << std::endl;
    cpu.reset();
    
    // Program: Demonstrate NOT operation
    std::vector<uint8_t> program5 = {
        0x15,  // 0: LDA #5    - Load 5 (0b0101) into A
        0xB0,  // 1: OUT A     - Output A=5
        0xE3,  // 2: NOT       - ~A = 0b1010 (10)
        0xB0,  // 3: OUT A     - Output A=10
        0xE3,  // 4: NOT       - ~A = 0b0101 (5) - back to original
        0xB0,  // 5: OUT A     - Output A=5
        0xF0   // 6: HLT       - Halt
    };
    
    cpu.loadProgram(program5);
    cpu.run();
    cpu.printState();
    
    std::cout << "\n=== Example 6: Shift Operations ===" << std::endl;
    cpu.reset();
    
    // Program: Demonstrate shift left and shift right
    std::vector<uint8_t> program6 = {
        0x13,  // 0: LDA #3    - Load 3 (0b0011) into A
        0xB0,  // 1: OUT A     - Output A=3
        0xE4,  // 2: SHL       - A << 1 = 0b0110 (6)
        0xB0,  // 3: OUT A     - Output A=6
        0xE4,  // 4: SHL       - A << 1 = 0b1100 (12)
        0xB0,  // 5: OUT A     - Output A=12
        0xE5,  // 6: SHR       - A >> 1 = 0b0110 (6)
        0xB0,  // 7: OUT A     - Output A=6
        0xE5,  // 8: SHR       - A >> 1 = 0b0011 (3)
        0xB0,  // 9: OUT A     - Output A=3
        0xF0   // A: HLT       - Halt
    };
    
    cpu.loadProgram(program6);
    cpu.run();
    cpu.printState();
    
    std::cout << "\n=== Example 7: Rotate Operations ===" << std::endl;
    cpu.reset();
    
    // Program: Demonstrate rotate left and rotate right
    std::vector<uint8_t> program7 = {
        0x19,  // 0: LDA #9    - Load 9 (0b1001) into A
        0xB0,  // 1: OUT A     - Output A=9
        0xE6,  // 2: ROL       - Rotate left = 0b0011 (3)
        0xB0,  // 3: OUT A     - Output A=3
        0xE6,  // 4: ROL       - Rotate left = 0b0110 (6)
        0xB0,  // 5: OUT A     - Output A=6
        0xE7,  // 6: ROR       - Rotate right = 0b0011 (3)
        0xB0,  // 7: OUT A     - Output A=3
        0xE7,  // 8: ROR       - Rotate right = 0b1001 (9) - back to start
        0xB0,  // 9: OUT A     - Output A=9
        0xF0   // A: HLT       - Halt
    };
    
    cpu.loadProgram(program7);
    cpu.run();
    cpu.printState();
    
    std::cout << "\n=== Example 8: Bit Masking (Practical Use) ===" << std::endl;
    cpu.reset();
    
    // Program: Extract lower 2 bits using AND
    std::vector<uint8_t> program8 = {
        0x1F,  // 0: LDA #15   - Load 15 (0b1111) into A
        0x23,  // 1: LDB #3    - Load 3 (0b0011) as mask into B
        0xE0,  // 2: AND       - A & B = 0b0011 (3) - extract lower 2 bits
        0xB0,  // 3: OUT A     - Output A=3
        0xF0   // 4: HLT       - Halt
    };
    
    cpu.loadProgram(program8);
    cpu.run();
    cpu.printState();
    
    return 0;
}