#pragma once
#include <FileMapping.hpp>
#include <cstdint>

#define INVALID_MEM_ADDR      1U
#define INITIALISATION_FALURE 2U

class Memory {
public:
	Memory(FileMapping& fm);
	uint8_t  ReadU8(uint64_t pos);
	uint16_t ReadU16(uint64_t pos);
	uint32_t ReadU32(uint64_t pos);
	uint64_t ReadU64(uint64_t pos);
	uint32_t GetError();

private:
	uint32_t err;
	FileMapping map;
};