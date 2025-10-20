#include "ALUControl.h"

ALUControl::ALUControl(Controller control, int func3, int func7) {
    this -> control = control;
    this -> func3 = func3;
    this -> func7 = func7;
}
void ALUControl::set_output() {
    // I-type, need to check func3 and func7
    if (control.ALUOp == 0x0) {
        // addI (00 00 )
        if (func3 == 0x0) {
            four_bit_output = 0x0;
        }
        // orI (00 01)
        else if(func3 == 0x6) {
            four_bit_output = 0x1;
        }
        // SLTIU (00 10)
        else if(func3 == 0x3) {
            four_bit_output = 0x2;
        }
        // LUI (00 11)
        else {
            four_bit_output = 0x3;
        }
    }
    // R-type, need to check func3 and func7
    else if (control.ALUOp == 0x1) {
        // SUB (01 00)
        if (func3 == 0x0) {
            four_bit_output = 0x4;
        }
        // AND (01 01)
        else if(func3 == 0x7) {
            four_bit_output = 0x5;
        }
        // RSA (01 10)
        else if(func3 == 0x5) {
            four_bit_output = 0x6;
        }
    }
    else if (control.ALUOp == 0x2) {
        // 10 00  (ADD)
        four_bit_output = 0x8;
    }
    else if(control.ALUOp == 0x3) {
        // 11 00 
        four_bit_output = 0xC;
    }
}

