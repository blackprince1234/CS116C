#include "ALUControl.h"
#include <iostream> 
using namespace std;
ALUControl::ALUControl(int aluOp, uint32_t func3, uint32_t func7, Controller& control): control(control) { 
    this -> aluOp = aluOp;
    this -> func3 = func3;
    this -> func7 = func7;
}
void ALUControl::set_output() {
    // I-type, need to check func3 and func7
    if (aluOp == 0x0) {
        // LUI (00 11)
        if(control.is_lui == 1) {
            four_bit_output = 0b0011;
        }
        // addI (00 00 )
        else if (func3 == 0x0) {
            four_bit_output = 0b0000;
        }
        // orI (00 01)
        else if(func3 == 0x6) {
            four_bit_output = 0b0001;
        }
        // SLTIU (00 10)
        else if(func3 == 0x3) {
            four_bit_output = 0b0010;
        }
    }
    // R-type, need to check func3 and func7
    else if (aluOp == 0x1) {
        if (func3 == 0x0) { // SUB (01 00)
            four_bit_output = 0b0100;
        }
        else if(func3 == 0x7) { // AND (01 01)
            four_bit_output = 0b0101;
        }
        else if(func3 == 0x5) { // SRA (01 10)
            four_bit_output = 0b0110;
        }
    }
    // For LW, LBU, SH, and SW
    else if (aluOp == 0x2) {
        // 10 00  (ADD)
        four_bit_output = 0b1000;
    }
    // Never called for some reason
    else if(aluOp == 0x3) {
        // cout << "EXTENDING" << endl;
        // 11 00  (SUB)
        four_bit_output = 0b1100;
    }
}