#include <Decoders.hpp>
#define CHECKED_READ(dest,state) {\
	dest = state->mem.ReadU8(state->pc);\
	if (state->mem.GetError()) { return 0; }\
	}


int decode_EB_GB(DecoderState* state, DecodedInstruction* ins) {
	ins->flags |= FLAGS_8_BIT;

	// We only need to read the ModR/M byte
	// state.pc points right at our ModR/M byte
	uint8_t n = 0;
	CHECKED_READ(n, state);

	ModRM modrm = ModRMLUT[n];
	//ins->dest = init8(modrm.reg);

	// We have the source now, go after the destination

	state->pc += 1;
	return 1;
}
