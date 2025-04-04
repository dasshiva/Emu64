// Emu64.cpp : Defines the entry point for the application.
//

#include <Memory.hpp>
#include <iostream>
using namespace std;

int main(int argc, const char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " [FILENAME]";
		return 1;
	}

	FileMapping fm(argv[1]);
	if (!fm.IsValid()) {
		std::cout << "Failed to map file to memory " << argv[1] << "\n";
		std::cout << "Error code = " << fm.GetError() << "\n";
		return 1;
	}

	Memory mem(fm);
	if (mem.GetError()) {
		std::cout << "Failed to initialise memory subsystem\n";
		std::cout << "Error code = " << mem.GetError() << "\n";
		return 1;
	}

	uint64_t pc = 0;
	while (true) {
		//std::cout << pc << "\n";
		uint8_t opc = mem.ReadU8(pc);
		if (mem.GetError()) {
			std::cout << "PAGE FAULT: Attempt to read invalid address "
				<< pc << "\n";
			return 1;
		}

		if (opc == 0xC3)
			break;
		pc += 1;
	}
	return 0;
}
