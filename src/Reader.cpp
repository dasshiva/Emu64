#include <Memory.hpp>

Memory::Memory(FileMapping& fm) : map(fm) {
	if (!fm.IsValid()) {
		err = INITIALISATION_FALURE;
		return;
	}

	err = 0;
}

#include <iostream>
uint8_t Memory::ReadU8(uint64_t pos) {
	if (pos >= map.Size()) {
		err = INVALID_MEM_ADDR;
		return 0;
	}

	uint8_t* mem = static_cast<uint8_t*>(map.GetMappedMemory());
	return mem[pos];
}

uint16_t Memory::ReadU16(uint64_t pos) {
	if (pos + 1 >= map.Size()) {
		err = INVALID_MEM_ADDR;
		return 0;
	}

	// Unaligned address
	if (pos % 2) 
		return ((uint16_t)ReadU8(pos + 1) << 8) | ReadU8(pos);
	else {
		uint16_t* mem = static_cast<uint16_t*>(map.GetMappedMemory());
		return mem[pos / 2];
	}
}

uint32_t Memory::ReadU32(uint64_t pos) {
	if (pos + 3 >= map.Size()) {
		err = INVALID_MEM_ADDR;
		return 0;
	}

	// Unaligned address
	if (pos % 4)
		return ((uint32_t)ReadU8(pos + 3) << 24) | 
				((uint32_t)ReadU8(pos + 2) << 16) | 
				((uint32_t)ReadU8(pos + 1) << 8) | ReadU8(pos);
	else {
		uint32_t* mem = static_cast<uint32_t*>(map.GetMappedMemory());
		return mem[pos / 4];
	}
}

uint64_t Memory::ReadU64(uint64_t pos) {
	if (pos + 7 >= map.Size()) {
		err = INVALID_MEM_ADDR;
		return 0;
	}

	// Unaligned address
	if (pos % 8) {
		return ((uint64_t)ReadU8(pos + 7) << 56) |
			((uint64_t)ReadU8(pos + 6) << 48) |
			((uint64_t)ReadU8(pos + 5) << 40) |
			((uint64_t)ReadU8(pos + 4) << 32) |
			((uint64_t)ReadU8(pos + 3) << 24) |
			((uint64_t)ReadU8(pos + 2) << 16) |
			((uint64_t)ReadU8(pos + 1) << 8) | ReadU8(pos);
	}
	else {
		uint64_t* mem = static_cast<uint64_t*>(map.GetMappedMemory());
		return mem[pos / 8];
	}
}

uint32_t Memory::GetError() {
	if (err) {
		if (err == INITIALISATION_FALURE)
			return map.GetError();
		return err;
	}

	return 0;
}