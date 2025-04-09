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

// A lot of cruft here, so let me explain
// By default c++ linkers will remove all symbols if they find anything
// to be unused. Even __atribute__((visibility("default"))) does not help
// the cause. Thus TableGen generates C code instead of C++  and we 
// compile with "-fvisibility=default" so that everything from 'C' code
// exported. The downside is that these symbols from TableGen don't get
// mangled nor can TableGen code itself refer to mangled symbol names
// Hence everything is wrapped inside extern "C"

// All of these functions return 0 on failure and 1 for success
extern "C" {
	int decode_EB_GB(DecoderState*, DecodedInstruction*);
}

extern "C" {
#include <tables.h>
}
