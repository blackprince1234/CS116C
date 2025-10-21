#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include "control.h"
#include "ALUControl.h"
#include "ALU.h"
using namespace std;
#pragma once



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
	uint8_t dmemory[1000000]; //data memory byte addressable in little endian fashion;
	uint8_t instMemory[4096];
	unsigned long PC; //pc 
	

public:
	CPU(char inmemory[4096]);
	int32_t registerFile[32]; 	// Access the value of each register
	unsigned long readPC();
	void incPC();
	void fetch();
	void decode();
	void printAll();
	// void memory();
	// void writeBack();
	uint32_t cur_instruction = 0;
	void printReg();
	struct IFID {
        uint32_t pc = 0;
        uint32_t instruction = 0;
    } ifidCurr;
};

// add other functions and objects here
