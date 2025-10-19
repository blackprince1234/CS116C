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
#define LUI 0x37
class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	uint8_t instMemory[4096];
	unsigned long PC; //pc 

public:
	CPU(char inmemory[4096]);
	unsigned long readPC();
	void incPC();
	void fetch();
	void decode();
	struct IFID {
        uint32_t pc = 0;
        uint32_t instruction = 0;
    } ifidCurr, ifidNext; // Structs for both "current" and "next"
	
};

// add other functions and objects here
