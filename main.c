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
    RESULT_NULL,
	INVALID_REGISTER,
	EXPECTED_REGNO,
	EXPECTED_REGISTER,
	EXPECTED_COMMA,
	STRAY_TOKEN,
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
        (*ins)++;
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

int GetRegister(char** ins, int32_t* ret) {
	if (**ins != 'r')
		return EXPECTED_REGISTER;

	(*ins)++;
	int32_t reg = -1;

	for (;;(*ins)++) {
        if (isdigit(**ins))
            reg = (reg == -1) ? (**ins - '0') : (reg * 10 + (**ins - '0'));
        else break;
    }

	if (reg == -1)
		return EXPECTED_REGNO;

	if (reg > 31)
		return INVALID_REGISTER;

	*ret = reg;
	return SUCCESS;

}

int AssembleAllRegs(char* ins, uint32_t* result) {
	if (SkipSpace(&ins, NO_EOF))
		return UNEXPECTED_EOF;

	int32_t rA = 0, rB = 0, rC = 0;
	int status = GetRegister(&ins, &rC);
	if (status)
		return status;

	if (SkipSpace(&ins, NO_EOF))
		return UNEXPECTED_EOF;

	if (*ins != ',')
		return EXPECTED_COMMA;

	ins++;
	if (SkipSpace(&ins, NO_EOF))
		return UNEXPECTED_EOF;

	status = GetRegister(&ins, &rA);
    if (status)
        return status;

    if (SkipSpace(&ins, NO_EOF))
        return UNEXPECTED_EOF;

    if (*ins != ',')
        return EXPECTED_COMMA;

    ins++;
    if (SkipSpace(&ins, NO_EOF))
        return UNEXPECTED_EOF;

	status = GetRegister(&ins, &rB);
    if (status)
        return status;

	SkipSpace(&ins, 0);
	if (*ins != '\0')
		return STRAY_TOKEN;

	*result |= (rA << 27) | (rB << 22) | (rC << 17);
    return SUCCESS;
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
