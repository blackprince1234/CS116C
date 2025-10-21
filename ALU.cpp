#include "ALU.h"
#include <iostream>
using namespace std;

ALU::ALU(
    uint32_t data1,
    uint32_t data2,
    uint32_t data3,
    ALUControl* aluControl,
    int32_t (&regFile)[32],     // Reference to 32-register file
    int instruction,
    uint8_t (&mem)[1000000],       // Reference to memory
    int& pc
): regs(regFile), memory(mem), pc(pc) // Member initializer list
{
    this->rs1 = data1;
    this->rs2 = data2;
    this->rd = data3;
    this->instruction = instruction;
    this->aluControl = aluControl;
}

int ALU::compute_immediate() {
    // For the immediate passed, need to check the immediate
    return (int32_t)instruction >> 20; 
}
auto ALU::getImmS() {
    int32_t imm = ((instruction >> 7) & 0x1F) | (((instruction >> 25) & 0x7F) << 5);
    // sign-extend 12-bit
    if (imm & 0x800) imm |= 0xFFFFF000;
    return imm;
};

void ALU::compute() {
    // Check if 
    // Is a load operation for sure ADDI, LW, SW, LBU 
    // BNE goes with SUB
    cout << "Executing" << endl;
    result = 0;
    switch (aluControl->four_bit_output) {
        // Case is weird 
        case 0x3: // LUI
            cout << "LUI Execution" << endl;
            result = (instruction & 0xFFFFF000);
            break;
        case 0x0: // ADDI
            cout << "AddI execution" << endl;
            cout << "IMMEDIATE " << compute_immediate() << endl;
            result = regs[rs1] + compute_immediate();
            // Check if should store in register file
            break;
        case 0x1: // ORI
            cout << "orI Execution" << endl;
            result = regs[rs1] | compute_immediate();
            break;

        case 0x2: // SLTIU
            cout << "SLTIU Execution" << endl;
            result = (static_cast<unsigned int>(regs[rs1]) < static_cast<unsigned int>(compute_immediate())) ? 1 : 0;
            break;
        
        case 0x4: // SUB
            cout << "SUB execution" << endl;
            result = regs[rs1] - regs[rs2];
            break;

        case 0x5: // AND
            cout << "AND execution" << endl;
            result = regs[rs1] & regs[rs2];
            break;

        case 0x6: // SRA (arithmetic right shift)
            cout << "SRA Execution" << endl;
            result = regs[rs1] >> regs[rs2]; // C++ preserves sign bit for signed ints
            break;

        case 0x8:  { // ADD (used for LW/SW)
            uint32_t opcode = instruction & 0x7F;

            if (opcode == 0x03) {           // LOAD group (LW/LBU)
                cout << "LOAD Execution" << endl;
                result = regs[rs1] + compute_immediate();                  // effective address
            } 
            else if (opcode == 0x23) {    // STORE group (SH/SW)
                cout << "STORE Execution" << endl;
                int32_t immS = getImmS();
                result = regs[rs1] + immS;                  // effective address
            } 
            break;
        }
        case 0xC: // Branch (BNE) or 
            if(aluControl->control.branch == 0) {   // SUB
                result = regs[rs1] - regs[rs2];
            }
            // Either BNE or JALR
            else {
                uint32_t opcode = instruction & 0x7F;
                if(opcode == 0x67) { //JALR
                    int32_t immI = compute_immediate();           // I-type imm
                    // Need to store result into register reg[rd] = pc + 4
                    result = pc + 4;                          
                    uint32_t target = (uint32_t)((int32_t)regs[rs1] + immI);
                    target &= ~1u;                                // clear bit 0
                    pc_target = target;
                    pc_write  = true;// tell CPU to jump
                }
                else if(opcode == 0x63) {   //BNE
                    if (regs[rs1] != regs[rs2]) {
                        
                        int32_t immB = 0;
                        immB |= ((instruction >> 31) & 0x1) << 12;       // imm[12]
                        immB |= ((instruction >> 25) & 0x3F) << 5;       // imm[10:5]
                        immB |= ((instruction >> 8) & 0xF) << 1;         // imm[4:1]
                        immB |= ((instruction >> 7) & 0x1) << 11;        // imm[11]

                        // Sign-extend to 32 bits
                        if (immB & (1 << 12)) {
                            immB |= 0xFFFFE000; // Fill upper bits with 1s if negative
                        }
                        pc_target = pc + immB;
                        pc_write  = true;                     // branch taken
                    }
                }
            }
            break;

        default:
            std::cerr << "Unknown ALU operation code: " << aluControl->four_bit_output << std::endl;
            return;
    }

}
void ALU::setPC() {
    if(pc_write){
        pc = pc_target;
    }
    else {
        pc += 4;
    }      
    pc_target = 0;
    pc_write = false;
}
// Either loading from the memory, writing to memory, or writing to registers
void ALU::writeBack() {
    cout << "Result: " << result << endl;
    // 1) LOADS
    if (aluControl->control.MemRead == 1) {
        uint32_t addr = static_cast<uint32_t>(result);
        uint32_t val  = 0;

        // LOAD group (Go to the effective address, store the value in result)
        if (aluControl->control.MemRead == 1) { 
            // LW
            if (aluControl->func3 == 0x2) {          
                result  = (uint32_t)memory[addr];
                result |= (uint32_t)memory[addr+1] << 8;
                result |= (uint32_t)memory[addr+2] << 16;
                result |= (uint32_t)memory[addr+3] << 24; 
                cout << "Loaded word: " << result << endl;
            }
             // LBU 
            else if (aluControl->func3 == 0x4) {  
                result = (uint32_t)memory[addr]; // zero-extend
                cout << "Loaded byte: " << result << endl;
            } 
        }
    }

    // 2) STORES (If MemWr == 1, needs to be writing something to memory, only store does it)
    if (aluControl->control.MemWr == 1) {
        uint32_t addr = static_cast<uint32_t>(result);
        uint32_t data = (uint32_t)regs[rs2];
        // SH
        if (aluControl->func3 == 0x1) {          
            memory[addr]   = (uint8_t)(data & 0xFF);
            memory[addr+1] = (uint8_t)((data >> 8) & 0xFF);
        } 
        // SW
        else if (aluControl->func3 == 0x2) {    
            memory[addr]   = (uint8_t)(data & 0xFF);
            memory[addr+1] = (uint8_t)((data >> 8) & 0xFF);
            memory[addr+2] = (uint8_t)((data >> 16) & 0xFF);
            memory[addr+3] = (uint8_t)((data >> 24) & 0xFF);
        } 
    }

    // Write to the register
    if(aluControl->control.RegWrite == 1) {
        regs[rd] = result;
    }
    // SW, SH
    else if(aluControl->control.branch == 1) {
        cout << "Processing jumps" << endl;
    }
}
// TODO: After computing the result, either add to the register file, or write it to the memory
// TODO: Also need to differentiate between LBUI and LW, and SW and SH.
// TODO: Need to calculate PC Counter for JALR and BNE.     
    