#include <string.h>
#include <iostream>
#include "ALUControl.h"
#include "control.h"

class ALU{
    public:
        ALU(uint32_t data1, uint32_t data2, uint32_t data3, ALUControl* aluControl, int32_t (&regFile)[32], int instruction, uint8_t (&mem)[1000000], int& pc);
        uint32_t rs1;
        uint32_t rs2;
        uint32_t rd;
        int result;
        int instruction;
        int immediate;
        // Might need to compute immediate value
        bool pc_write;
        int &pc;
        int pc_target;
        ALUControl* aluControl;
        int32_t (&regs)[32];
        uint8_t (&memory)[1000000];
        void compute();
        void writeBack();
        void setPC();
        int compute_immediate();
        auto getImmS();
};