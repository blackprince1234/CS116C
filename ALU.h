#include <string.h>
#include <iostream>
#include "ALUControl.h"
#include "control.h"

class ALU{
    public:
        ALU(int data1, int data2, int data3, ALUControl* aluControl, int& pc);
        int data1;
        int data2;
        int data3;
        ALUControl* aluControl;
};