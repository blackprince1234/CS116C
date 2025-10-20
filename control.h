#include <string.h>
#include <iostream>
#pragma once


class Controller {
    public:
        // Takes in 6 bit instruction (op code)
        Controller(int instruction);
        int is_lui;
        int branch;
        int MemRead;
        int MemToReg;
        int ALUOp;
        int ALUSrc;
        int MemWr;
        int RegWrite;	
};