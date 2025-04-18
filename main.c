#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

enum Status {
    SUCCESS = 0,
    UNKNOWN_OPCODE,
    UNEXPECTED_EOF,
    ARG_NULL,
    RESULT_NULL
};

typedef int (*AssembleFunc)(char*, uint32_t*);
struct Instruction {
    const char* name;
    AssembleFunc func;
    uint32_t meta;
};

int AssembleAllRegs(char*, uint32_t*);

const struct Instruction instrs[] = {
    { "add", AssembleAllRegs, ((0x31 << 11) | 0x3A) },
    { NULL, NULL, 0 }
};

enum SSFlags {
    NO_EOF = (1 << 0)
};

int SkipSpace(char** ins, int flags) {
    while (isspace(**ins)) {
        *ins++;
    }
    
    if ((flags & NO_EOF) && (**ins == '\0'))
        return UNEXPECTED_EOF;
    
    return SUCCESS;
}

int AssembleGeneric(char* ins, uint32_t* result) {
    if (!ins)
        return ARG_NULL;

    if (!result)
        return RESULT_NULL;
     
    if (SkipSpace(&ins, NO_EOF))
        return UNEXPECTED_EOF;

    char opcode[10] = {0};
    for (int i = 0; i < 10; i++, ins++) {
        if (*ins == '\0')
            return UNEXPECTED_EOF;

        if (isalpha(*ins))
            opcode[i] = *ins;
        else 
            break;
    }

    if (opcode[9] != '\0')
        return UNKNOWN_OPCODE;

    int idx = -1;
    for (int i = 0; instrs[i].name; i++) {
        if (strcmp(instrs[i].name, opcode) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1)
        return UNKNOWN_OPCODE;

    *result = instrs[idx].meta;
    return instrs[idx].func(ins, result);
}

int AssembleAllRegs(char* ins, uint32_t* result) {
    return SUCCESS;
}

uint32_t encode(char* ins) {
    while (isspace(*ins)) {
        ins++;
    }

    if (*ins == '\0')
        return 0xFFFFFFFF;

    char opcode[10] = {0};
    for (int i = 0; i < 10; i++, ins++) {
        if (*ins == '\0')
            return 0xFFFFFFFF;

        if (isalpha(*ins))
            opcode[i] = *ins;
        else 
            break;
    }

    while (isspace(*ins)) {
        ins++;
    }

    if (*ins == '\0')
        return 0xFFFFFFFF;

    if (*ins != 'r')
        return 0xFFFFFFFF;

    int rC = -1;
    ins++;
    for (;;ins++) {
        if (*ins == '\0')
            return 0xFFFFFFFF;

        if (isdigit(*ins))
            rC = (rC == -1) ? (*ins - '0') : (rC * 10 + (*ins - '0'));
        else break;
    }

    if (rC == -1 || rC > 31)
        return 0xFFFFFFFF;

    while (isspace(*ins)) {
        ins++;
    }

    if (*ins == '\0')
        return 0xFFFFFFFF;

    if (*ins != ',')
        return 0xFFFFFFFF;

    ins++;
    while (isspace(*ins)) {
        ins++;
    }

    if (*ins == '\0')
        return 0xFFFFFFFF;

    if (*ins != 'r')
        return 0xFFFFFFFF;

    int rA = -1;
    ins++;
    for (;;ins++) {
        if (*ins == '\0')
            return 0xFFFFFFFF;

        if (isdigit(*ins))
            rA = (rA == -1) ? (*ins - '0') : (rA * 10 + (*ins - '0'));
        else break;
    }

    if (rA == -1 || rA > 31)
        return 0xFFFFFFFF;

    while (isspace(*ins)) {
        ins++;
    }

    if (*ins == '\0')
        return 0xFFFFFFFF;

    if (*ins != ',')
        return 0xFFFFFFFF;
    ins++;

    while (isspace(*ins)) {
        ins++;
    }

    if (*ins == '\0')
        return 0xFFFFFFFF;

    if (*ins != 'r')
        return 0xFFFFFFFF;

    int rB = -1;
    ins++;
    for (;;ins++) {
        if (*ins == '\0') // Line ends right after last register
            break;

        if (isdigit(*ins))
            rB = (rB == -1) ? (*ins - '0') : (rB * 10 + (*ins - '0'));
        else break;
    }

    if (rB == -1 || rB > 31)
        return 0xFFFFFFFF;

    while (isspace(*ins)) {
        ins++;
    }

    if (*ins != '\0')
        return 0xFFFFFFFF;

    if (strcmp(opcode, "add") != 0)
        return 0xFFFFFFFF;

    uint32_t ret = rA << 27 | rB << 22 | rC << 17 | 0x31 << 11
       | 0x3A; 
    return ret;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: %s [INSN_1] [INSN_2] ...[INSN_N]\n", argv[0]);
        return 1;
    }

    char** insts = (argv + 1);
    while (*insts) {
        char* inst = *insts;
        uint32_t result = 0;
        int ret = AssembleGeneric(inst, &result);
        if (ret) {
            printf("Error code = %d\n", ret);
            return 1;
        }
        else
            printf("0x%x\n", result);
        insts++;
    }
    return 0;
}
