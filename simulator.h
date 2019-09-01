#include "header.h"
#include "reader.h"
#include "memory.h"
#include "decode.h"

class Simulator{
    private:
        char* elfBinary;
        void resetSimulator();
        Reader *r;
        Memory *mem;
        Decode *d;
    public:
        Simulator(char*);
        void startSimulation();

};


/*
void resetSimulator()
{
	sparcRegisters.psr.cwp = 0;
	sparcRegisters.psr.c = 0;
	sparcRegisters.psr.v = 0;
	sparcRegisters.psr.z = 0;
	sparcRegisters.psr.n = 0;
	sparcRegisters.pc = 0;
	sparcRegisters.npc = 4;
        
     //   setIUErrorMode(0);
}*/

