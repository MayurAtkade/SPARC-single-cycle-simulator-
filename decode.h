#include "header.h"

struct instruction_fields{
    unsigned short op, op2, op3, opf;
};

class Decode{
    private:
        struct instruction_fields sparc_instruction_fields;
        char* getIntegerRegisterName(unsigned long registerIdentifier);
        char* getFloatingRegisterName(unsigned long registerIdentifier);
        char* getCoProcessorRegisterName(unsigned long registerIdentifier);
        char* getAddress(unsigned long rs1, unsigned long rs2, unsigned long i, unsigned long simm13, int registerTypeIdentifier);
        char* getReg_Or_Imm(unsigned long rs2, unsigned long i, unsigned long simm13, int registerTypeIdentifier);
    public:
        char* decodeInstruction(char* cpuInstruction, unsigned long regPC);

};