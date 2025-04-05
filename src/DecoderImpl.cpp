#include <Decoders.hpp>
#define CHECKED_READ(dest,state) {\
	dest = state->mem.ReadU8(state->pc);\
	if (state->mem.GetError()) { return 0; }\
	}

struct ModRM {
	uint8_t mod;
	uint8_t rm;
	uint8_t reg;
	ModRM(uint8_t n) : mod((n & 0b11000000) >> 6), rm(n & 0b111),
		reg((n & 0b111000) >> 3) {}
};

int decode_eb_gb(DecoderState* state, DecodedInstruction* ins) {
	ins->flags |= FLAGS_8_BIT;

	// We only need to read the ModR/M byte
	// state.pc points right at our ModR/M byte
	uint8_t n = 0;
	CHECKED_READ(n, state);

	ModRM modrm(n);
	ins->dest = init8(modrm.reg);

	// We have the source now, go after the destination

	state->pc += 1;
	return 1;
}