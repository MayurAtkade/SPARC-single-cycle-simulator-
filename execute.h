#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdio.h>
#include <math.h>
#include <fenv.h>
#include <string.h>
#include <limits.h>

#include "memory.h"
#include "constants.h"
#include "decode.h"
#include "register.h"
#include "other.h"


#define ICC_CARRY 				20
#define ICC_OVERFLOW            21
#define ICC_ZERO				22
#define ICC_NEGATIVE            23
#define SIGN_BIT				31
#define FLOAT_EQUAL              0
#define FLOAT_LESSER             1
#define FLOAT_GREATER            2
#define FLOAT_UNORDERED          3




struct last_Instruction_Info
{
    unsigned long regPC;
    char disassembledInstruction[50];
    char cpuInstruction[4];
};


class Execute{
    private:
    public:
        struct last_Instruction_Info lastInstructionInfo;
        int executeInstruction(char* disassembledInstruction, Memory *memory, Register *sregister);
        unsigned long getAddressValue(char tokens[][20], unsigned short* index, Register *sregister);
        unsigned long getReg_Or_ImmValue(char* reg_or_imm, Register *sregister);
        void updateICCAdd(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, Register *sregister);
        void updateICCSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, Register *sregister);
        void updateICCMulLogical(unsigned long regRD, Register *sregister);
        void updateICCDiv(unsigned long regRD, short isOverflow, Register *sregister);
        void updateFCC(unsigned short fcc, Register *sregister);
        int executeNextInstruction(Memory memory, Register *sregister);
        unsigned short taggedAddSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, unsigned short isTVOpcode, Register *sregister);
};

#endif
