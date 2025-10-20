#include "ALUControl.h"
#include <iostream> 
using namespace std;
ALUControl::ALUControl(int aluOp, uint32_t func3, uint32_t func7): control(0) { 
    this -> aluOp = aluOp;
    this -> func3 = func3;
    this -> func7 = func7;
}
void ALUControl::set_output() {
    // I-type, need to check func3 and func7
    if (aluOp == 0x0) {
        // cout << "I-type" << endl;
        // cout << "Func3: " << func3 << endl;
        
        // LUI (00 11)
        if(control.is_lui == 1) {
            // cout << "LUI Operation" << endl;
            four_bit_output = 0x3;
        }
        // addI (00 00 )
        else if (func3 == 0x0) {
            // cout << "Add operation" << endl;
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
    }
    // R-type, need to check func3 and func7
    else if (aluOp == 0x1) {
        // SUB (01 00)
        if (func3 == 0x0) {
            four_bit_output = 0x4;
        }
        // AND (01 01)
        else if(func3 == 0x7) {
            four_bit_output = 0x5;
        }
        // SRA (01 10)
        else if(func3 == 0x5) {
            four_bit_output = 0x6;
        }
    }
    // For LW, LBU, SH, and SW
    else if (aluOp == 0x2) {
        // 10 00  (ADD)
        four_bit_output = 0x8;
    }
    else if(aluOp == 0x3) {
        // 11 00  (SUB)
        four_bit_output = 0xC;
    }
}

