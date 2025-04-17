#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

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
        printf("0x%x\n", encode(inst));
        insts++;
    }
    return 0;
}
