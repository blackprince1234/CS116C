#include <string.h>
#include <iostream>


class Controller {
    public:
        // Takes in 6 bit instruction (op code)
        Controller(std::string instruction);
        int branch;
        int MemRead;
        int MemToReg;
        int ALUOp;
        int ALUSrc;
        int MemWr;
        int RegWrite;	
};