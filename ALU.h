#include <string.h>
#include <iostream>
#include "ALUControl.h"
#include "control.h"

class ALU{
    public:
        ALU(uint32_t data1, uint32_t data2, uint32_t data3, ALUControl* aluControl, int32_t (&regFile)[32], int instruction, int (&mem)[4096]);
        uint32_t rs1;
        uint32_t rs2;
        uint32_t rd;
        int result;
        int instruction;
        int immediate;
        // Might need to compute immediate value
        
        ALUControl* aluControl;
        int32_t (&regs)[32]; // Member variable: reference to register file
        int (&memory)[4096]; // Reference to the memory array
        void compute();
        void writeBack();
        int compute_immediate();
};