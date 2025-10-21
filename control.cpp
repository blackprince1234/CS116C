#include "control.h"
#include <iostream>
using namespace std;

// Takes in the OPcode (last 7 bits)
Controller::Controller(int instruction) {
    // Assign member variables based on the instruction
    if(instruction == 0b0010011) {  // I-type
        RegWrite = 1, ALUSrc = 1;
        branch = 0, MemRead = 0, MemWr = 0, MemToReg = 0, is_lui = 0;
        ALUOp = 0x0;
    }
    else if(instruction == 0b0110111) { // LUI
        RegWrite = 1, is_lui = 1, ALUSrc = 1;
        branch = 0, MemRead = 0, MemWr = 0, MemToReg = 0;
        ALUOp = 0x0;
    }
    else if (instruction == 0b0110011) {    // R-Type
        RegWrite = 1;
        ALUSrc = 0, branch = 0, MemRead = 0, MemWr = 0, MemToReg = 0, is_lui = 0;
        ALUOp = 0b01;
    }
    else if (instruction == 0b0000011) {    // LW
        RegWrite = ALUSrc = MemRead = MemToReg = 1;
        branch = MemWr = is_lui = 0;
        ALUOp = 0b10;    // ADD
    }
    else if (instruction == 0b0100011) {    // SW
        ALUSrc = MemWr = 1;
        branch = MemRead = MemToReg = RegWrite = is_lui = 0;
        ALUOp = 0b10;        // ADD
    }   
    else if (instruction == 0b1100011) {     // BEQ
        branch = 1;
        is_lui = MemRead = MemToReg = RegWrite = MemWr = ALUSrc = 0;
        ALUOp = 0b11;    // SUBTRACT
    }
    else if (instruction == 0b1100111) {    // JALR
        branch = 1;
        is_lui = MemRead = MemToReg = RegWrite = MemWr = ALUSrc= 0;
        ALUOp = 0b11;    // SUBTRACT
    }
    //TODO: Add JALR
}

// For r-type, ALUOp = 00
// For i-type, ALUOp = 01
// For 