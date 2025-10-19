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
}
// So memory is only actually used for LW and SW, and for operations like addi, and subi, it will be called in writeback() where registerFile of the rd is updated with the new data
/*

Exactly! You got it right. Here's a quick recap with some emphasis:

Memory stage (memory()) is only actively involved in load (LW) and store (SW) instructions:

For LW, it reads from dataMem and loads data into the pipeline register to later write back to the register file.

For SW, it writes data from a register into dataMem at the calculated address.

For arithmetic or logic instructions like ADDI, SUB, ADD, etc., the memory stage does nothing special — the ALU results are passed through unchanged.

The writeback stage (writeback()) is where the register file gets updated:

For load instructions, it writes the data read from memory into the destination register.

For other ALU operations, it writes the ALU result into the destination register.

Store instructions do not write back to the register file.*/
void CPU::fetch() {
	std::cout << "Current PC: " << PC << std::endl;

	// Fetch the next 4 bytes. Each instruction is 4 bytes long
	if (PC + 3 < 4096) {  // assuming instMemory[4096] is a raw array
		uint32_t byte1 = static_cast<uint8_t>(instMemory[PC]);
		uint32_t byte2 = static_cast<uint8_t>(instMemory[PC + 1]);
		uint32_t byte3 = static_cast<uint8_t>(instMemory[PC + 2]);
		uint32_t byte4 = static_cast<uint8_t>(instMemory[PC + 3]);
		
		// std::cout << "Bytes: "
		//           << std::hex << std::uppercase << std::setfill('0')
		//           << "0x" << std::setw(2) << byte1 << " "
		//           << "0x" << std::setw(2) << byte2 << " "
		//           << "0x" << std::setw(2) << byte3 << " "
		//           << "0x" << std::setw(2) << byte4
		//           << std::dec << std::endl;

	    ifidCurr.instruction = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;
		// cout << "Next instruction: " << ifidCurr.instruction << endl;
	}
}

void CPU::decode() {
	uint32_t opcode = ifidCurr.instruction & 0x7f;
    uint32_t func3 = (ifidCurr.instruction >> 12) & 0x7;
    uint32_t func7 = (ifidCurr.instruction >> 25) & 0x7f;
    uint32_t rs1 = (ifidCurr.instruction >> 15) & 0x1f;
    uint32_t rs2 = (ifidCurr.instruction >> 20) & 0x1f;

	std::cout << "Instruction (32-bit): " << std::bitset<32>(ifidCurr.instruction) << '\n';
    std::cout << "Opcode     (7-bit):  " << std::bitset<7>(opcode) << '\n';
    std::cout << "Func3      (3-bit):  " << std::bitset<3>(func3) << '\n';
    std::cout << "Func7      (7-bit):  " << std::bitset<7>(func7) << '\n';
    std::cout << "RS1        (5-bit):  " << std::bitset<5>(rs1) << '\n';
    std::cout << "RS2        (5-bit):  " << std::bitset<5>(rs2) << '\n';

	// Check which op code matches
	/* 	
		ADDI, LUI, ORI, SLTIU, SRA, SUB, AND, LBU, LW, SH, SW, BNE, JALR
	*/

	// If addi -> Check the op code, func3, func7, rs1, rst2, etc. 
	// Intermediate register
	if (opcode == ITYPE) {
		if (0x0 == func3) { cout << "Add operation" << endl; }
        else if (0x6 == func3) { cout << "Or operation" << endl; }
		else if (0x3 == func3) {cout << "SLTIU operation" << endl;}
	}
	// Using registers for computation
	else if (opcode == RTYPE) {
		// SRA = 101
		if(0x5 == func3) {
			cout << "SRA Operation" << endl;
		}
		// Can either be add or subtract
		else if(func3 == 0x0) {
			if(func7 == 0x0) {
				cout << "Add operation (RTYPE)" << endl;
			}
			else if (func7 == 0x20) {
				cout << "Subtract operation (RTYPE)" << endl;
			}
			else {cout << "Operation not recognized-- RTYPE" << endl;}
		}
		else {cout << "Operation not recognized-- RTYPE" << endl;}
	}
	else if(opcode == LUI) {
		cout << "LUI Operation" << endl;
	}

}	
unsigned long CPU::readPC()
{
	return PC;
}
void CPU::incPC()
{
	PC+=4;
}

// Add other functions here ... 