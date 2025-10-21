#include "CPU.h"
#include <iomanip> // For testing, remove later
using namespace std;
CPU::CPU(char inmemory[4096])
{
	std::memcpy(this->instMemory, inmemory, 4096);
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
	for (int i = 0; i < 32; i++) {
		registerFile[i] = 0;
	}
	
}
void CPU::fetch() {
	// Fetch the next 4 bytes. Each instruction is 4 bytes long
	if (PC + 3 < 4096) {  // assuming instMemory[4096] is a raw array
		uint32_t byte1 = static_cast<uint8_t>(instMemory[PC]);
		uint32_t byte2 = static_cast<uint8_t>(instMemory[PC + 1]);
		uint32_t byte3 = static_cast<uint8_t>(instMemory[PC + 2]);
		uint32_t byte4 = static_cast<uint8_t>(instMemory[PC + 3]);
	    ifidCurr.instruction = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;
        cur_instruction = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;
	}
}

void CPU::decode() {
	uint32_t opcode = ifidCurr.instruction & 0x7f;
    uint32_t func3 = (ifidCurr.instruction >> 12) & 0x7;
    uint32_t func7 = (ifidCurr.instruction >> 25) & 0x7f;
    uint32_t rs1 = (ifidCurr.instruction >> 15) & 0x1f;
    uint32_t rs2 = (ifidCurr.instruction >> 20) & 0x1f;
    uint32_t rd = (ifidCurr.instruction >> 7) & 0x1F;

	std::cout << "Instruction (32-bit): " << std::bitset<32>(cur_instruction) << '\n';
    Controller controller(opcode);                              // Pass in the last 7 bits to the controller
    ALUControl aluControl(controller.ALUOp, func3, func7, controller);      // Pass to the aluController
    aluControl.control = controller;
    aluControl.set_output();
    // int data1, int data2, int data3, ALUControl* aluControl, int32_t (&regFile)[32]
    ALU alu(rs1, rs2, rd, &aluControl, registerFile, ifidCurr.instruction, dmemory, PC);
    alu.compute();
    alu.writeBack();
    // printReg();
    // TODO: Update the PC (need to be passed by reference)
    alu.setPC();
}

// Execute the actual task

unsigned long CPU::readPC()
{
	return PC;
}


// Increment counter based
void CPU::incPC()
{
	PC += 4;
}

void CPU::printReg() {
	std::cout << "--- Registers ---" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << "x" << std::setw(2) << std::setfill('0') << i 
                << ": " << std::dec << registerFile[i] 
                << " (0x" << std::hex << registerFile[i] << ")" << std::dec;

        if (i != 31) std::cout << " | ";
    }
    std::cout << std::endl;
}
void CPU::printAll() {
    // std::cout << "=== CPU STATE ===" << std::endl;

    // // Print Program Counter
    // std::cout << "PC: " << PC << std::endl;

    // // Print Registers
    // std::cout << "--- Registers ---" << std::endl;
    // for (int i = 0; i < 32; ++i) {
    //     std::cout << "x" << std::setw(2) << std::setfill('0') << i 
    //               << ": " << std::dec << registerFile[i] 
    //               << " (0x" << std::hex << registerFile[i] << ")" << std::dec << std::endl;
    // }

    // // Print part of Data Memory (first 16 bytes)
    // std::cout << "--- Data Memory (first 16 bytes) ---" << std::endl;
    // for (int i = 0; i < 16; ++i) {
    //     std::cout << "dmemory[" << i << "]: " << dmemory[i] << std::endl;
    // }

    // std::cout << "==================" << std::endl;
}