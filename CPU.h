#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;


// class instruction { // optional
// public:
// 	bitset<32> instr;//instruction
// 	instruction(bitset<32> fetch); // constructor

// };

// Define different types of operations
#define ZERO 0x0 
#define RTYPE 0x33
#define ITYPE 0x13
#define LOADWORD 0x3
#define STOREWORD 0x23
// #define LUI 0x37
#define JUMP 0x67
class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	uint8_t instMemory[4096];
	unsigned long PC; //pc 
	int32_t registerFile[32]; 	// Access the value of each register

public:
	CPU(char inmemory[4096]);
	unsigned long readPC();
	void incPC();
	void fetch();
	void decode();
	void execute();
	void printAll();
	void memory();

	// Different types of operations
	enum class Op {
		ADDI,
		LUI,
		ORI,
		SLTIU,
		SRA,
        SUB,
		AND, 
		LBU,                   
        LW,
		SH,
        SW,
		BNE,
		JALR, 
		ZE,   
		ERROR,     
    };
// ADDI, LUI, ORI, SLTIU, SRA, SUB, AND, LBU, LW, SH, SW, BNE, JALR



	struct IFID {
        uint32_t pc = 0;
        uint32_t instruction = 0;
    } ifidCurr, ifidNext; // Structs for both "current" and "next"
	
	struct IDEX {
        uint32_t pc = 0;
        int32_t readData1 = 0;
        int32_t readData2 = 0;
        uint32_t rd = 0;
        int32_t immediate = 0;
        Op operation = Op::ZE;
    } idexCurr, idexNext; // Structs for both "current" and "next"

	struct EXMEM {
        uint32_t pc = 0;
        int32_t aluResult = 0;
        int32_t readData2 = 0;
        uint32_t rd = 0;
        Op operation = Op::ZE;
		bool takeBranch = false;   uint32_t branchTarget = 0;
		bool doJump    = false;    uint32_t jumpTarget  = 0;
    } exmemCurr, exmemNext; // Structs for both "current" and "next"

    // Information representing the register block between the Memory
    // and Writeback stages.
    struct MEMWB {
        uint32_t rd = 0;
        int32_t aluResult = 0;
        int32_t memData = 0;
        Op operation = Op::ZE;
    } memwbCurr, memwbNext; // Structs for both "current" and "next"
};

// add other functions and objects here
