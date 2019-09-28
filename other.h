#ifndef OTHER_H
#define OTHER_H

#include "header.h"
#include <math.h>

union 
{
    unsigned long floatToHex;
    float hexToFloat;
}convertFloat;



union 
{
    unsigned long doubleToHex[2];
    double hexToDouble;
}convertDouble;



class Other{
    private:
        
    public:
        int getBit(unsigned long bitStream, int position);
        unsigned long setBit(unsigned long bitStream, int position);
        unsigned long clearBit(unsigned long bitStream, int position);
        unsigned long toggleBit(unsigned long bitStream, int position);
        char* showBits(unsigned long bitStream, int startPosition, int endPosition);
        void displayWord(char* cpuInstruction, int isInstruction);
        int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment);
        int is_register_mis_aligned(char* sparcRegister);
        int isSNaN32(float float32);
        int isQNaN32(float float32);
        int isSNaN64(double float64);
        int isQNaN64(double float64);
};

#endif
