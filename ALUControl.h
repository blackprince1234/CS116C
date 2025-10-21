#include <string.h>
#include <iostream>
#include "control.h"
#pragma once
class ALUControl {
    public:
        ALUControl(int aluOp, uint32_t func3, uint32_t func7, Controller& control);
        Controller control;
        // Need additional 2 variables if ALUOp = RType or IType
        int func3;
        int func7;
        int aluOp;
        // Output the 4 bit output
        int four_bit_output;
        void set_output();
};