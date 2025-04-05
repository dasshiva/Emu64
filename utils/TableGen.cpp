#include <fstream>
#include <map>
#include <string>
#define	FILE_INACCESSIBLE 1

int main(int argc, const char** argv) {
	if (argc != 2) 
		return 1;
	std::ofstream file;
	file.open(argv[1]);
	if (!file.is_open()) 
		return FILE_INACCESSIBLE;
	std::map<int, std::string> bltns;
	bltns[0] = "decode_eb_gb";
	bltns[10] = "decode_eb_gb";
	bltns[20] = "decode_eb_gb";
	bltns[30] = "decode_eb_gb";
	file << "#pragma once" << std::endl;
	file << "// auto generated DO NOT EDIT MANUALLY" << std::endl;
	file << "// Edit utils/TableGen.cpp if you need changes here" 
		<< std::endl;
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
	file.close();
	return 0;
}
