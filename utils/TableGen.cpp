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

/*void addBuiltin(std::map<int, std::string>& bltns, std::string toappend, int
fst){ bltns[fst] = toappend;
}

template<typename... Args>
void addBuiltin(std::map<int, std::string>& bltns, std::string toappend, int
fst, Args... n) { bltns[fst] = toappend; addBuiltin(bltns, toappend, n...);
}

int main(int argc, const char** argv) {
        if (argc != 2)
                return 1;
        std::ofstream file;
        file.open(argv[1]);
        if (!file.is_open())
                return FILE_INACCESSIBLE;

        
        // Add builtin decoder functions here as needed
        std::map<int, std::string> bltns;
        addBuiltin<int>(bltns, "decode_eb_gb", 0, 0x10, 0x20, 0x30);

        file << "// auto generated DO NOT EDIT MANUALLY" << std::endl;
        file << "// Edit utils/TableGen.cpp if you need changes here"
                << std::endl;

        // Some duplicate function prototypes will be created
        // but most compilers do not care about some extra
        // function signature declarations
        for (auto it = bltns.begin(); it != bltns.end(); it++) {
                file << "int " << it->second << "(struct DecoderState*, struct
DecodedInstruction*);\n";
        }

        // Declare fundamental types
         file << "const DecodeFunc Decoders[] = {"
<< std::endl; for (int i = 0; i < 256; i++) { auto fn = bltns.find(i); if (fn !=
bltns.end()) file << fn->second << "," << std::endl; else file << "((void*)0),"
<< std::endl;
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
        file << "};\n";
        file.close();
        return 0;
} */

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

    hfile << "struct DecoderState;\n";
    hfile << "struct DecodedInstruction;\n\n";
    hfile << "typedef int (*DecodeFunc)(struct DecoderState*," 
        << "struct DecodedInstruction*);" << std::endl;
    hfile << "extern const DecodeFunc Decoders[];\n" << std::endl;

    sfile << "// AUTO GENERATED - DO NOT EDIT MANUALLY\n";
    sfile << "// Consider editing utils/file.cfg or utils/TableGen.cpp instead\n";
    sfile << "#include \"" << std::string(argv[2]) + ".h" << "\" \n";

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

    hfile << "#ifdef __cplusplus" << std::endl;
    hfile << "} " << std::endl;
    hfile << "#endif" << std::endl;

    hfile.close();
    sfile.close();
    FreeConfig(config);
    return 0;
}
