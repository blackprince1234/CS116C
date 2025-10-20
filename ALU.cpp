// Need to directly write to the registers and update pc counter as well

#include "ALU.h"
#include <iostream>
using namespace std;
// ALU::ALU(uint32_t data1, uint32_t data2, uint32_t data3, ALUControl* aluControl, int32_t (&regFile)[32], int instructionm, int (&mem)[4096]): regs(regFile), memory(mem){
//     this->rs1 = data1;
//     this-> rs2 = data2;
//     this->rd = data3;
//     this->instruction = instruction;
//     this->aluControl = aluControl;
// }
ALU::ALU(
    uint32_t data1,
    uint32_t data2,
    uint32_t data3,
    ALUControl* aluControl,
    int32_t (&regFile)[32],     // Reference to 32-register file
    int instruction,
    int (&mem)[4096]            // Reference to memory
): regs(regFile), memory(mem)    // Member initializer list
{
    this->rs1 = data1;
    this->rs2 = data2;
    this->rd = data3;
    this->instruction = instruction;
    this->aluControl = aluControl;
}

int ALU::compute_immediate() {
    // For the immediate passed, need to check the immediate
    return (instruction >> 20) & 0xFFF;
}


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
                int32_t immS = ((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F);
                result = regs[rs1] + immS;                  // effective address
            } 
            break;
        }
        case 0xC: // Branch (BNE)
            // if (rs1 != rs2) {
            //     pc += rd; // branch taken
            // }
            break;

        default:
            std::cerr << "Unknown ALU operation code: " << aluControl->four_bit_output << std::endl;
            return;
    }

}
void ALU::writeBack() {
    cout << "Result: " << result << endl;
    
    // LW, LBU (Read the memory at address[result])
    // if (aluControl -> control.MemRead == 1) {
    //     // Result orignally has the computed memory address.
    //     if(aluControl->func3 == 0x2) {      // LW
    //         cout << "Executing lw" << endl;
    //         uint32_t word_index = result >> 2;
    //         uint32_t byte_offset = result & 0x3;
        
    //         // Read the 32-bit word from memory
    //         result = memory[word_index];
    //     }
    //     else if (aluControl -> func3 == 0x4) {  // LBU
    //         cout << "Executing LBU" << endl;
    //         int shift_amount = result * 8;
    //         uint32_t byte_value = (result >> shift_amount) & 0xFF;
            
    //         // Update result for the write-back stage
    //         result = byte_value;
    //         result = memory[result];
    //     }
    // }
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
            }
             // LBU 
            else if (aluControl->func3 == 0x4) {  
                result = (uint32_t)memory[addr]; // zero-extend
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
    else if(aluControl->control.MemWr == 1) {
        cout << "SW and SH" << endl;
    }
}
// TODO: After computing the result, either add to the register file, or write it to the memory
// TODO: Also need to differentiate between LBUI and LW, and SW and SH.
// TODO: ADDI Not correclty executing (-32)