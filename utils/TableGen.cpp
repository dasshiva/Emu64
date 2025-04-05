#include <cstdint>
#include <fstream>
#include <map>
#include <ostream>
#include <string>
#define	FILE_INACCESSIBLE 1

void addBuiltin(std::map<int, std::string>& bltns, std::string toappend, int fst){
	bltns[fst] = toappend;
}

template<typename... Args>
void addBuiltin(std::map<int, std::string>& bltns, std::string toappend, int fst, Args... n) {
	bltns[fst] = toappend;
	addBuiltin(bltns, toappend, n...);
}

int main(int argc, const char** argv) {
	if (argc != 2) 
		return 1;
	std::ofstream file;
	file.open(argv[1]);
	if (!file.is_open()) 
		return FILE_INACCESSIBLE;

	file << "struct DecoderState;\n";
	file << "struct DecodedInstruction;\n";

	// Add builtin decoder functions here as needed
	std::map<int, std::string> bltns;
	addBuiltin<int>(bltns, "decode_eb_gb", 0, 0x10, 0x20, 0x30);

	file << "#pragma once" << std::endl;
	file << "// auto generated DO NOT EDIT MANUALLY" << std::endl;
	file << "// Edit utils/TableGen.cpp if you need changes here" 
		<< std::endl;

	// Some duplicate function prototypes will be created
	// but most compilers do not care about some extra 
	// function signature declarations
	for (auto it = bltns.begin(); it != bltns.end(); it++) {
		file << "int " << it->second << "(DecoderState*, DecodedInstruction*);\n";
	}

	// Declare fundamental types
	file << "typedef int (*DecodeFunc)(DecoderState*, DecodedInstruction*);" << std::endl;
	file << "const DecodeFunc Decoders[] = {" << std::endl;
	for (int i = 0; i < 256; i++) {
		auto fn = bltns.find(i);
		if (fn != bltns.end()) 
			file << fn->second << "," << std::endl;
		else 
			file << "nullptr," << std::endl;
	}
	
	file << "};" << std::endl;

	std::map<int, std::string> opcodes;
	// Add opcodes here as needed
	opcodes[0] = "ADD_EB_GB";
	opcodes[0x10] = "ADC_EB_GB";
	opcodes[0x20] = "AND_EB_GB"; 
	opcodes[0x30] = "XOR_EB_GB";
	for (auto it = opcodes.begin(); it != opcodes.end(); it++) {
		file << "#define " << it->second << " (" << it->first << ")\n";
	}

	file << "const unsigned short OpcodeMap1[16][16] = {" << std::endl;
	for (uint8_t i = 0; i < 16; i++) {
		file << "{";
		for (uint8_t j = 0; j < 16; j++) {

			auto op = opcodes.find((i << 4) | j);
			if (op != opcodes.end()) 
				file << op->second;
			else
				file << "0xFFFF";
			if (j != 15) 
				file << ",";
		}

		file << "}";
		if (i != 15) 
			file << ",";
		file << "\n";
	}
	file << "};";
	file.close();
	return 0;
}
