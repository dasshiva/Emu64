// Emu64.cpp : Defines the entry point for the application.
//

#include <FileMapping.hpp>
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
	return 0;
}
