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


	// Update the IDEX structure
	idexNext.pc = ifidCurr.pc;
    idexNext.readData1 = registerFile[rs1];
    idexNext.readData2 = registerFile[rs2];
    idexNext.rd = (ifidCurr.instruction >> 7) & 0x1f;
    idexNext.immediate = (int32_t) ifidCurr.instruction >> 20; 

	// Check which op code matches
	/* 	
		ADDI, LUI, ORI, SLTIU, SRA, SUB, AND, LBU, LW, SH, SW, BNE, JALR
	*/

	if (opcode == ITYPE) {		// Intermediate computation
		if (0x0 == func3) { idexNext.operation = Op::ADDI; }
        else if (0x6 == func3) { idexNext.operation = Op::ORI; }
		else if (0x3 == func3) { idexNext.operation = Op::SLTIU; }
	}
	// SUB, AND, SRA
	else if (opcode == RTYPE) {	// Using registers for computation
		// SRA = 101
		if(0x5 == func3 && func7 == 0x20) {
			idexNext.operation = Op::SRA;
		}
		else if(func3 == 0x0 && func7 == 0x20) { idexNext.operation = Op::SUB; }
		else if(func3 == 0x7) {
			idexNext.operation = Op::AND;
		}
		else {cout << "Operation not recognized-- RTYPE" << endl;}
	}
	else if (opcode == LOADWORD) {		// LW and LBU
		if (func3 == 0x2) {
			idexNext.operation = Op::LW;
		} else if (func3 == 0x4) {
			idexNext.operation = Op::LBU;
		}
	}
	else if (opcode == STOREWORD) {		// SW and SH
		// auto imm11_5 = (int32_t)(ifidCurr.instruction & 0xfe000000);
		// auto imm4_0 = (int32_t)((ifidCurr.instruction & 0xf80) << 13);
		// idexNext.immediate = (imm11_5 + imm4_0) >> 20;
		auto inst = ifidCurr.instruction;
		int32_t immS = (int32_t)(((inst >> 25) << 5) | ((inst >> 7) & 0x1F));
		immS = (immS << 20) >> 20;
		idexNext.immediate = immS;

		if (func3 == 0x2) {
			idexNext.operation = Op::SW;
		} else if (func3 == 0x1) {
			idexNext.operation = Op::SH;
		}
	}
	else if (opcode == JUMP && func3 == 0x0) {		// JALR
		idexNext.operation = Op::JALR;
	}
	else if (opcode == 0x63 && func3 == 0x1) {
		idexNext.operation = Op::BNE;
	}
	else if(opcode == 0x37) {
		idexNext.operation = Op::LUI;
	}
}	

// Execute the actual task
void CPU::execute() {
	exmemNext.pc = idexCurr.pc;
    exmemNext.operation = idexCurr.operation;
    exmemNext.readData2 = idexCurr.readData2;
    exmemNext.rd = idexCurr.rd;

	exmemNext.takeBranch   = false;
    exmemNext.branchTarget = 0;
    exmemNext.doJump       = false;
    exmemNext.jumpTarget   = 0;

	switch(idexCurr.operation) {
		case Op::ADDI:
            exmemNext.aluResult = idexCurr.readData1 + idexCurr.immediate;
            break;
		case Op::LUI:
			exmemNext.aluResult = static_cast<uint32_t>(idexCurr.immediate);
	    	break;
		case Op::ORI:
            exmemNext.aluResult = idexCurr.readData1 | idexCurr.immediate;
            break;
		case Op::SLTIU:
			exmemNext.aluResult = (static_cast<uint32_t>(idexCurr.readData1) < static_cast<uint32_t>(idexCurr.immediate)) ? 1 : 0;
			break;
		 case Op::SRA: {
            // Arithmetic right shift: sign-propagating
            uint32_t shamt = idexCurr.readData2 & 0x1F;         // RV32: 5-bit
            int32_t  s     = static_cast<int32_t>(idexCurr.readData1);
            exmemNext.aluResult = static_cast<uint32_t>(s >> shamt);
            break;
        }
        case Op::SUB:
            exmemNext.aluResult = idexCurr.readData1 - idexCurr.readData2;
            break;
        case Op::AND:
            exmemNext.aluResult = idexCurr.readData1 & idexCurr.readData2;
            break;
        case Op::LW:
        case Op::SW:
		case Op::SH:
		case Op::LBU:
            exmemNext.aluResult = idexCurr.readData1 + idexCurr.immediate;
            break;
		case Op::BNE:
            if (idexCurr.readData1 != idexCurr.readData2) {
                exmemNext.takeBranch   = true;
                exmemNext.branchTarget = idexCurr.pc + idexCurr.immediate; // B-type offset already sign-extended
            }
            break;

        case Op::JALR: {
            exmemNext.aluResult = idexCurr.pc + 4;
            uint32_t t = static_cast<uint32_t>(
                static_cast<int32_t>(idexCurr.readData1) + idexCurr.immediate
            );
            exmemNext.jumpTarget = t & ~1u;
            exmemNext.doJump     = true;
            break;
        }
        case Op::ZE:
            exmemNext.aluResult = ZERO;
            break;

        case Op::ERROR:
            break;
    }
}
// Store in the memory
void CPU::memory() {
	memwbNext.rd = exmemCurr.rd;
    memwbNext.aluResult = exmemCurr.aluResult;
    memwbNext.operation = exmemCurr.operation;

    int32_t lByte1, lByte2, lByte3, lByte4;
    uint8_t sByte1, sByte2, sByte3, sByte4;
    switch(exmemCurr.operation) {
        case Op::LW:
            // Fetch 4 bytes from the data memory in little endian form.
            lByte1 = dmemory[exmemCurr.aluResult];
            lByte2 = dmemory[exmemCurr.aluResult + 1];
            lByte3 = dmemory[exmemCurr.aluResult + 2];
            lByte4 = dmemory[exmemCurr.aluResult + 3];

            // Convert to big endian and store as the aluResult
            memwbNext.memData = (lByte4 << 24) + (lByte3 << 16) + (lByte2 << 8) + lByte1;
            break;
		case Op::LBU: 
			lByte1 = dmemory[exmemCurr.aluResult];
			memwbNext.memData = static_cast<uint32_t>(lByte1) & 0xFF;
            break;

        case Op::SW:
            // Separate bytes of readData2
            sByte1 = (exmemCurr.readData2 >> 24) & 0xff000000;
            sByte2 = (exmemCurr.readData2 >> 16) & 0xff0000;
            sByte3 = (exmemCurr.readData2 >> 8) & 0xff00;
            sByte4 = exmemCurr.readData2 & 0xff;
            // Store in dataMem in little endian form
            dmemory[exmemCurr.aluResult] = sByte4;
            dmemory[exmemCurr.aluResult + 1] = sByte3;
            dmemory[exmemCurr.aluResult + 1] = sByte2;
            dmemory[exmemCurr.aluResult + 1] = sByte1;
            break;
		case Op::SH:
            // Separate bytes of readData2
			sByte1 = (exmemCurr.readData2 >> 8) & 0xFF;  // high byte
			sByte2 = exmemCurr.readData2 & 0xFF;         // low byte

			// Store in data memory in little endian order
			dmemory[exmemCurr.aluResult]     = sByte2;   // lower byte
			dmemory[exmemCurr.aluResult + 1] = sByte1;   // higher byte
            break;

        default:
            break;
    }
}
unsigned long CPU::readPC()
{
	return PC;
}
// Increment counter based
void CPU::incPC()
{
	// PC+=4;
	if (exmemCurr.doJump)      PC = exmemCurr.jumpTarget;
	else if (exmemCurr.takeBranch) PC = exmemCurr.branchTarget;
	else                        PC += 4;
}

void CPU::printAll() {
    std::cout << "=== CPU STATE ===" << std::endl;

    // Print Program Counter
    std::cout << "PC: " << PC << std::endl;

    // Print Registers
    std::cout << "--- Registers ---" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << "x" << std::setw(2) << std::setfill('0') << i 
                  << ": " << std::dec << registerFile[i] 
                  << " (0x" << std::hex << registerFile[i] << ")" << std::dec << std::endl;
    }

    // Print part of Data Memory (first 16 bytes)
    std::cout << "--- Data Memory (first 16 bytes) ---" << std::endl;
    for (int i = 0; i < 16; ++i) {
        std::cout << "dmemory[" << i << "]: " << dmemory[i] << std::endl;
    }

    std::cout << "==================" << std::endl;
}