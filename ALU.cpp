// Need to directly write to the registers and update pc counter as well

#include "ALU.h"

ALU::ALU(int data1, int data2, int data3, ALUControl* aluControl, int& pc) {
    this->data1 = data1;
    this-> data2 = data2;
    this->data3 = data3;
    this->aluControl = aluControl;
}

void compute() {
    // Check if 
    // Is a load operation for sure ADDI, LW, SW, LBU 
    // BNE goes with SUB
    if( aluControl->four_bit_output == 0x8) {
        if (aluControl -> )
    }
    // 0xC (Is a branch operation)
    else if (aluControl-> four_bit_output = 0xC) {

    }
    else {
        // Need to check all possibilities
    }
}