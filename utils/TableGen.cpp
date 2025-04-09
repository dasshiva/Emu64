#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <cstring>

extern "C" {
#include "cfg/cfg_parse.h"
}

#define FILE_INACCESSIBLE 1

int processOpcode(Vector* vec, std::map<unsigned, std::string>& opmap
		, std::map<unsigned, std::string>& dmap) {
    PrimitiveValue *n1, *n2;
    n1 = GetElement(vec, 0);
    n2 = GetElement(vec, 1);

    if (!n1 || !n2) {
	std::cout << "*Opcode vector too small\n";
	return -1;
    }

    if (n1->Type != NUMBER_TYPE || n2->Type != NUMBER_TYPE) {
	std::cout << "m and n must be integers\n";
	return -1;
    }

    uint64_t m = n1->Number, n = n2->Number;
    if (vec->Length != ((2*n + 1) * m + 2)) {
	std::cout << "*Opcode vector has invalid number of elements\n";
	return -1;
    }

    for (uint64_t op = 2; op < vec->Length; op++) {
	PrimitiveValue* name = GetElement(vec, op);
	if (name->Type != STRING_TYPE) {
	    std::cout << "Instruction name must be a string\n";
	    return -1;
	}

	op++;
	for (uint64_t fm_desc = 0; fm_desc < 2*n; fm_desc += 2) {
	    PrimitiveValue* fm = GetElement(vec, op + fm_desc);
	    if (fm->Type != STRING_TYPE) {
		std::cout << "Instruction Format name must be string\n";
		return -1;
	    }

	    PrimitiveValue* opcode = GetElement(vec, op + fm_desc + 1);
	    if (opcode->Type != NUMBER_TYPE) {
		std::cout << "Format opcode must be number\n";
		return -1;
	    }

	    opmap[opcode->Number] = std::string(name->String) + "_" + 
		    fm->String;
	    dmap[opcode->Number] = fm->String;
	}

	op += (2 * n) - 1;
    }

    return 0;
}

int processRegs(Value* val, std::ofstream& hfile) {
    Vector *bregs = val->Array;
    for (uint64_t i = 0; i < bregs->Length; i++) {
        PrimitiveValue *pv = GetElement(bregs, i);
        if (pv->Type != STRING_TYPE) {
            std::cout << "All Values in *Regs must be strings\n";
            return -1;
        }

        hfile << "#define REG_" << pv->String << " (" << i << ")" << "\n";
    }
    hfile << std::endl;

    return 0;
}

int main(int argc, const char **argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [CONF].cfg [OUTPUT].h\n";
        return 1;
    }

    Config *config;
    int status = ParseConfig(argv[1], &config);
    if (status < 0) {
        std::cout << "ERROR: " << ErrToString(status) << "\n";
        return 1;
    }

    // hfile is the header, sfile is the source
    std::ofstream hfile, sfile;
    hfile.open(std::string(argv[2]) + ".h");
    sfile.open(std::string(argv[2]) + ".c");

    if (!hfile.is_open()) {
        std::cout << "Failed to access output file " << argv[2] << ".h \n";
        return 1;
    }

    if (!sfile.is_open()) {
        std::cout << "Failed to access output file " << argv[2] << ".c \n";
        return 1;
    }

    hfile << "// AUTO GENERATED - DO NOT EDIT MANUALLY\n";
    hfile << "// Consider editing utils/file.cfg or utils/TableGen.cpp instead\n";
    hfile << "#ifdef __cplusplus" << std::endl;
    hfile << "extern \"C\" { " << std::endl;
    hfile << "#endif" << std::endl; 
    hfile << "#pragma once\n\n";
    hfile << "#include <stdint.h>\n";

    hfile << "struct DecoderState;\n";
    hfile << "struct DecodedInstruction;\n\n";
    hfile << "typedef int (*DecodeFunc)(struct DecoderState*," 
        << "struct DecodedInstruction*);" << std::endl;
    hfile << "extern const DecodeFunc Decoders[];\n" << std::endl;

    sfile << "// AUTO GENERATED - DO NOT EDIT MANUALLY\n";
    sfile << "// Consider editing utils/file.cfg or utils/TableGen.cpp instead\n";
    sfile << "#include \"" << std::string(argv[2]) + ".h" << "\" \n";

    std::map<unsigned, std::string> OpcodeMap, DecoderMap;
    ConfigEntry *ce = config->List;
    for (uint64_t idx = 0; idx < config->Entries; idx++) {
        uint64_t len = std::strlen(ce->Key);
        if (len > 4)  { // 4 = strlen("Regs")
            if (ce->Key[len-4] == 'R' && ce->Key[len-3] == 'e' &&
                    ce->Key[len-2] == 'g' && ce->Key[len-1] == 's') {
                if (ce->Type != ARRAY_TYPE) {
                    std::cout << "All *Regs keys must be arrays\n";
                    return 1;
                }

                if (processRegs(ce->Value, hfile) < 0) 
                    return 1;
            }
        }

	if (len > 6) { // 6 = strlen("Opcode")
	    if (ce->Key[len-6] == 'O' && ce->Key[len-5] == 'p' &&
                    ce->Key[len-4] == 'c' && ce->Key[len-3] == 'o' &&
		    ce->Key[len-2] == 'd' && ce->Key[len - 1] == 'e') {
		if (ce->Type != ARRAY_TYPE) {
		    std::cout << "All *Opcode keys must be arrays\n";
		    return 1;
		}

		if (processOpcode(ce->Value->Array, OpcodeMap, DecoderMap) < 0)
		    return 1;

	    }
	}

        if (std::strcmp(ce->Key, "Decoders") == 0) {
            if (ce->Type != ARRAY_TYPE) {
                std::cout << "Decoders key must be an array\n";
                return 1; 
            }

            Vector* decoders = ce->Value->Array;
            sfile << "\n// Decoder Function declarations\n";
            for (uint64_t dec = 0; dec < decoders->Length; dec++) {
                PrimitiveValue* pv = GetElement(decoders, dec);
                if (pv->Type != STRING_TYPE) {
                    std::cout << "All decoder values must be strings\n";
                    return 1;
                }

                sfile << "int " << pv->String << " (struct DecoderState*, " <<
                    "struct DecodedInstruction*); " << std::endl;
            }
        }
        ce = ce->Next;
    }

    for (auto it = OpcodeMap.begin(); it != OpcodeMap.end(); it++) {
	hfile << "#define " << it->second << " " << '(' << it->first << ')'
		<< std::endl;
    }
    hfile << "extern const uint16_t OpcodeMap1[];\n";
    hfile << "#ifdef __cplusplus" << std::endl;
    hfile << "} " << std::endl;
    hfile << "#endif" << std::endl;

    sfile << "const uint16_t OpcodeMap1[] = {\n";
    for (unsigned i = 0; i < 256; i++) {
	auto it = OpcodeMap.find(i);
	if (it != OpcodeMap.end()) 
	   sfile << it->second << ",\n";
	else 
	   sfile << "0xFFFF,\n";
    }
    sfile << "};\n";

    sfile << "const DecodeFunc Decoders[] = {\n";
    for (unsigned i = 0; i < 256; i++) {
	auto it = DecoderMap.find(i);
	if (it != DecoderMap.end()) 
	   sfile << ("decode_" + it->second) << ",\n";
	else 
	   sfile << "((void*)0),\n";
    }
    sfile << "};\n";


    hfile.close();
    sfile.close();
    FreeConfig(config);
    return 0;
}
