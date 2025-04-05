#include <fstream>

#define	FILE_INACCESSIBLE 1

int main(int argc, const char** argv) {
	std::ofstream file;
	file.open(argv[1]);
	if (!file.is_open()) 
		return FILE_INACCESSIBLE;

	file << "#pragma once" << std::endl;
	file << "struct DecoderState;" << std::endl;
	file << "typedef int (*DecodeFunc)(DecoderState*);" << std::endl;
	file << "const DecodeFunc Decoders[] = {" << std::endl;
	for (int i = 0; i < 256; i++) {
		file << "nullptr," << std::endl;
	}
	file << "};" << std::endl;
	file.close();
	return 0;
}