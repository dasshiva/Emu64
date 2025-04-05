// Emu64.cpp : Defines the entry point for the application.
//
#include <Memory.hpp>
#include <Decoders.hpp>
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

	DecoderState ds(0, mem);
	while (true) {
		uint8_t opc = mem.ReadU8(ds.pc);
		DecodedInstruction di;
		di.pc = ds.pc;
		di.opcode = opc;

		if (mem.GetError()) {
			std::cout << "PAGE FAULT: Attempt to read invalid address 0x"
				<< std::hex << ds.pc << "\n";
			return 1;
		}

		if (OpcodeMap1[opc >> 4][opc & 0b1111] == 0xFFFF) {
			std::cout << "INVALID OPCODE: 0x" << std::hex << (uint32_t)opc
				<< " at pc = 0x" << ds.pc;
			return 1;
		}

		uint64_t orig_pc = ds.pc;
		ds.pc += 1; // skip the opcode as it already has been recorded
		if (!Decoders[opc](&ds, &di)) {
			std::cout << "INVALID INSTRUCTION AT: 0x" << std::hex << orig_pc << "\n";
			return 1;
		}

		if (opc == 0xC3)
			break;
	}

	return 0;
}
