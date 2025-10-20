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

	    ifidNext.instruction = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;
        ifidNext.pc = PC;
	}
}

void CPU::decode() {
	uint32_t opcode = ifidCurr.instruction & 0x7f;
    uint32_t func3 = (ifidCurr.instruction >> 12) & 0x7;
    uint32_t func7 = (ifidCurr.instruction >> 25) & 0x7f;
    uint32_t rs1 = (ifidCurr.instruction >> 15) & 0x1f;
    uint32_t rs2 = (ifidCurr.instruction >> 20) & 0x1f;
    uint32_t rd = (ifidCurr.instruction >> 7) & 0x1F;

	// std::cout << "Instruction (32-bit): " << std::bitset<32>(ifidCurr.instruction) << '\n';
    // std::cout << "Opcode     (7-bit):  " << std::bitset<7>(opcode) << '\n';
    // std::cout << "Func3      (3-bit):  " << std::bitset<3>(func3) << '\n';
    // std::cout << "Func7      (7-bit):  " << std::bitset<7>(func7) << '\n';
    // std::cout << "RS1        (5-bit):  " << std::bitset<5>(rs1) << '\n';
    // std::cout << "RS2        (5-bit):  " << std::bitset<5>(rs2) << '\n';


	// Update the IDEX structure
	idexNext.pc = ifidCurr.pc;
    idexNext.readData1 = registerFile[rs1];
    idexNext.readData2 = registerFile[rs2];
    idexNext.rd = (ifidCurr.instruction >> 7) & 0x1f;
    idexNext.immediate = ((int32_t)ifidCurr.instruction) >> 20;  // Arithmetic shift


	if (opcode == ITYPE) {		
		if (0x0 == func3) { 
            idexNext.operation = Op::ADDI; 
            cout << "ADDI " << rd << " " << rs1 <<  " " << idexNext.immediate << endl;
        }
        else if (0x6 == func3) { 
            cout << "ORI " << rd << " " << rs1 <<  " " << idexNext.immediate << endl;
            idexNext.operation = Op::ORI; 
        }
		else if (0x3 == func3) { 
            cout << "SLTIU " << rd << " " << rs1 <<  " " << idexNext.immediate << endl;
            idexNext.operation = Op::SLTIU; 
        }
	}
	// SUB, AND, SRA // rs1 - rs2
	else if (opcode == RTYPE) {	// Using registers for computation
		// SRA = 101
		if(func3 == 0x5 && func7 == 0x20) {
			idexNext.operation = Op::SRA;
            cout << "RSA " << rd << ", " << rs1 << "," << rs2 << endl;
		}
		else if(func3 == 0x0 && func7 == 0x20) { 
            idexNext.operation = Op::SUB; 
            cout << "SUB " << rd << ", " << rs1 << "," << rs2 << endl;
        }
		else if(func3 == 0x7) {
			idexNext.operation = Op::AND;
            cout << "AND " << rd << ", " << rs1 << "," << rs2 << endl;
		}
	}
	else if (opcode == LOADWORD) {		// LW and LBU
		if (func3 == 0x2) {
            
			idexNext.operation = Op::LW;
		} else if (func3 == 0x4) {
			idexNext.operation = Op::LBU;
		}
	}

	else if (opcode == STOREWORD) {	
		auto inst = ifidCurr.instruction;
        uint32_t imm_11_5 = (inst >> 25) & 0x7F;  // 7 bits immediate
        uint32_t imm_4_0  = (inst >> 7) & 0x1F;   // 5 bits immediate
        uint32_t imm = (imm_11_5 << 5) | imm_4_0;        // Combine into 12-bit value
		idexNext.immediate = imm;
        // SW rd, offset(rs1)
		if (func3 == 0x2) {
			idexNext.operation = Op::SW;
            cout << "SW " << rs2 << " " << imm << " " << rs1 << endl;
            
		} else if (func3 == 0x1) {
            cout << "SH " << rs2 << " " << imm << " " << rs1 << endl;
			idexNext.operation = Op::SH;
		}

	}
	else if (opcode == JUMP && func3 == 0x0) {		// JALR
		idexNext.operation = Op::JALR;
	}
    // BNE
	else if (opcode == 0x63 && func3 == 0x1) {
        idexNext.operation = Op::BNE;
        uint32_t inst = ifidCurr.instruction;
        int32_t immB =
            ((inst >> 31) & 0x1) << 12 |   // imm[12]
            ((inst >> 7)  & 0x1) << 11 |   // imm[11]
            ((inst >> 25) & 0x3F) << 5 |   // imm[10:5]
            ((inst >> 8)  & 0xF) << 1;     // imm[4:1]
        // sign-extend 13-bit and make it byte address (already <<1 above)
        immB = (immB << 19) >> 19;
        idexNext.immediate = immB;
    }
    // LUI
	else if(opcode == 0x37) {
		idexNext.operation = Op::LUI;
        // Overwrite immediate (format is different)
        idexNext.immediate = ifidCurr.instruction >> 12;
        cout << "LUI " << idexNext.rd  << " " << idexNext.immediate << endl;
	}
}

// For debugging: print individual instruction
void printInstruction(string method, string src, string dest) {



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
    std::cout << "Current operation: " << static_cast<int>(idexCurr.operation) << std::endl;
	switch(idexCurr.operation) {
		case Op::ADDI:
            exmemNext.aluResult = idexCurr.readData1 + idexCurr.immediate;
            cout << "Execute: Add operation" << endl;
            break;
		case Op::LUI:
			exmemNext.aluResult = static_cast<uint32_t>(idexCurr.immediate);
            cout << "Execute: LUI" << endl;
	    	break;
		case Op::ORI:
            exmemNext.aluResult = idexCurr.readData1 | idexCurr.immediate;
            cout << "Execute: OR" << endl;
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
    switch(memwbNext.operation) {
        case Op::LW:            // Fetch 4 bytes from the data memory in little endian form.
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

        case Op::SW: {
			cout << "Store word called" << endl;
            // Separate bytes of readData2
            uint32_t v = exmemCurr.readData2;
            uint32_t a = exmemCurr.aluResult;
            dmemory[a + 0] = static_cast<uint8_t>(v & 0xFF);
            dmemory[a + 1] = static_cast<uint8_t>((v >> 8)  & 0xFF);
            dmemory[a + 2] = static_cast<uint8_t>((v >> 16) & 0xFF);
            dmemory[a + 3] = static_cast<uint8_t>((v >> 24) & 0xFF);
            break;
        }
		case Op::SH:
			cout << "Store half called" << endl;
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
void CPU::writeBack() {
    // registerFile[memwbCurr.rd] = memwbCurr.aluResult;
    switch (memwbCurr.operation) {
        case Op::LW:
        case Op::LBU:
            if (memwbCurr.rd != 0) registerFile[memwbCurr.rd] = memwbCurr.memData;
            break;
        case Op::ADDI:
        case Op::LUI:
        case Op::ORI:
        case Op::SLTIU:
        case Op::SRA:
        case Op::SUB:
        case Op::AND:
        case Op::JALR:
            if (memwbCurr.rd != 0) registerFile[memwbCurr.rd] = memwbCurr.aluResult;
            break;
        default:
            // STOREs/BRANCHes don’t write a register
            break;
    }
    // Ensure x0 is hard-wired to 0
    registerFile[0] = 0;
}

// Increment counter based
void CPU::incPC()
{
	// PC+=4;
	if (exmemCurr.doJump)      PC = exmemCurr.jumpTarget;
	else if (exmemCurr.takeBranch) PC = exmemCurr.branchTarget;
	else                        PC += 4;
}
void CPU::tick() {
    memwbCurr = memwbNext;
    exmemCurr = exmemNext;
    idexCurr  = idexNext;
    ifidCurr  = ifidNext;

    // clear "Next" for the next cycle
    memwbNext = {};
    exmemNext = {};
    idexNext  = {};
    ifidNext  = {};
    
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