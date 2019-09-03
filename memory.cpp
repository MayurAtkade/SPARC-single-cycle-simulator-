#include "memory.h"

Memory::Memory(){
    memory=new int[65536];
    curMemory=0;
    lastMemory=0;
}

void writeToMemory(unsigned long memoryAddress)
{
    this->memory[curMemory] = memoryAddress;
    curMemory++;
}
void Memory::set_lastMemory(unsigned long l){
     lastMemory=l;
}

unsigned long Memory::get_lastMemory(){
    return lastMemory;
}
void Memory::set_curMemory(unsigned long c){
    curMemory=c;

}
unsigned long Memory::get_curMemory(){
    return curMemory;
}


char* Memory::readWordAsString(unsigned long memoryAddress)
{
	char* cpuInstruction = (char*)malloc(4);
	cpuInstruction[0] = memory[memoryAddress++];          // Read the first byte.
	cpuInstruction[1] = memory[memoryAddress++];          // Read the second byte.
	cpuInstruction[2] = memory[memoryAddress++];          // Read the third byte.
	cpuInstruction[3] = memory[memoryAddress];            // Read the fourth byte.
	return cpuInstruction;
}
