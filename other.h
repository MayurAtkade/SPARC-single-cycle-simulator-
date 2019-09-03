#include "header.h"

class Other{
    private:
        
    public:
        int getBit(unsigned long bitStream, int position);
        unsigned long setBit(unsigned long bitStream, int position);
        unsigned long clearBit(unsigned long bitStream, int position);
        unsigned long toggleBit(unsigned long bitStream, int position);
        char* showBits(unsigned long bitStream, int startPosition, int endPosition);
        void displayWord(char* cpuInstruction, int isInstruction);

};


