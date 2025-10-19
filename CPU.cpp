#include "CPU.h"
#include <iomanip> // For testing, remove later

CPU::CPU(char inmemory[4096])
{
	std::memcpy(this->instMemory, inmemory, 4096);
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
	
}

void CPU::fetch() {
	std::cout << "Current PC: " << PC << std::endl;

	// Fetch the next 4 bytes. Each instruction is 4 bytes long
	if (PC + 3 < 4096) {  // assuming instMemory[4096] is a raw array
		uint32_t byte1 = static_cast<uint8_t>(instMemory[PC]);
		uint32_t byte2 = static_cast<uint8_t>(instMemory[PC + 1]);
		uint32_t byte3 = static_cast<uint8_t>(instMemory[PC + 2]);
		uint32_t byte4 = static_cast<uint8_t>(instMemory[PC + 3]);
		
		std::cout << "Bytes: "
		          << std::hex << std::uppercase << std::setfill('0')
		          << "0x" << std::setw(2) << byte1 << " "
		          << "0x" << std::setw(2) << byte2 << " "
		          << "0x" << std::setw(2) << byte3 << " "
		          << "0x" << std::setw(2) << byte4
		          << std::dec << std::endl;
	}

}

unsigned long CPU::readPC()
{
	return PC;
}
void CPU::incPC()
{
	PC+=4;
}

// Add other functions here ... 