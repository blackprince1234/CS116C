#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include<fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	char instMem[4096];


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile >> line && i < 4096) {
	    unsigned int val = 0;
	    stringstream(line) >> std::hex >> val; // convert "37" → 0x37
	    instMem[i++] = static_cast<char>(val & 0xFF);
	}

	int maxPC= i/4; 

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU = CPU(instMem);  // call the approriate constructor here to initialize the processor...  

	// Pass in instruction to the CPU

	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 
	
	bool done = true;
	// myCPU.printAll();
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		myCPU.fetch();        // fill ifidNext
		myCPU.decode();       // produce idexNext
		myCPU.execute();      // produce exmemNext
		myCPU.memory();       // produce memwbNext
		myCPU.writeBack();    // <-- actually commit to registerFile using memwbCurr
		myCPU.incPC();        // update PC with branch/jump/seq
		myCPU.tick();         // latch *Next into *Curr and clear *Next
		// myCPU.printReg();
		if (myCPU.readPC() > maxPC)
			break;
	}
	int a0 = 0;
	int a1 = 0;  
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	// myCPU.printAll();
	cout << "(" << a0 << "," << a1 << ")" << endl;
	
	return 0;

}