#pragma once
#include <cstdint>
#include <Memory.hpp>
struct DecoderState {
	uint64_t pc;
	Memory&   mem;
	uint32_t flags;
	DecoderState(uint64_t pc, Memory& mem) : pc(pc), mem(mem), flags(0) {}
};

/*
* meaning of flags:
* flags[1:0] : width of instruction (0 = 8, 1 = 16, 2 = 32, 3 = 64)
* flags[2]   : Does the instruction reference memory? (0 = False, 1 = True)
* flags[3]   : If flags[2] == 1, then 0 = source in memory, 1 = dest in memory
*/
struct DecodedInstruction {
	uint64_t pc;
	uint16_t opcode;
	uint8_t  src;
	uint8_t  dest;
	uint8_t  flags;
	uint32_t dispOrImmediate;
};

#define FLAGS_8_BIT (0)
#define FLAGS_16_BIT (1)
#define FLAGS_32_BIT (2)
#define FLAGS_64_BIT (3)

#define init8(val) ((val) << FLAGS_8_BIT)
enum ByteRegs {
	AL = init8(0),
	CL = init8(1),
	DL = init8(2),
	BL = init8(3),
	AH = init8(4),
	CH = init8(5),
	DH = init8(6),
	BH = init8(7),
};

// All of these functions return 0 on failure and 1 for success
int decode_eb_gb(DecoderState*, DecodedInstruction*);

// This must always be after everything in the file because the auto 
// generated tables.h refers to all of these functions.
#include <tables.h>
