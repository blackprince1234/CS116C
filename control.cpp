#include "control.h"


Controller::Controller(std::string instruction) {
    // Assign member variables based on the instruction
    // I-type (Add LUI as well)
    if(instruction == "0010011" || instruction == "0010111") {
        RegWrite = 1, ALUSrc = 1;
        branch = 0, MemRead = 0, MemWr = 0, MemToReg = 0;
        ALUOp = 0x0;
    }
    else if (instruction == "0110011") {
        // R-Type
        RegWrite = 1;
        ALUSrc = 0, branch = 0, MemRead = 0, MemWr = 0, MemToReg = 0;
        ALUOp = 0x1;
    }
    else if (instruction == "0000011") {
        // LW
        RegWrite = ALUSrc = MemRead = MemToReg = 1;
        ALUOp = 0x2;    // ADD
    }
    else if (instruction == "0100011") {
        // SW
        ALUSrc = MemWr = 1;
        ALUOp = 0x2;        // ADD
    }
    else if (instruction == "1100011") {
        // BEQ
        branch = 1;
        ALUOp = 0x3;    // SUBTRACT
    }
    //TODO: Add JALR
}

// For r-type, ALUOp = 00
// For i-type, ALUOp = 01
// For 