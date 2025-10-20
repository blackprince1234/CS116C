#include <string.h>
#include <iostream>
#include "control.h"

class ALUControl {
    public:
        ALUControl(Controller control, int func3, int func7);
        Controller control;
        // Need additional 2 variables if ALUOp = RType or IType
        int func3;
        int func7;

        // Output the 4 bit output
        int four_bit_output;
        void set_output();
};