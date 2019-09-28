#include "memory.h"

Memory::Memory(){
    memory=new int[65536];
    curMemory=0;
    lastMemory=0;
}

void Memory::writeToMemory(unsigned long memoryAddress)
{
    memory[curMemory] = memoryAddress;
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

int Memory::writeByte(unsigned long memoryAddress, char byte)
{
     memory[memoryAddress] = byte;
}

char Memory::readByte(unsigned long memoryAddress)
{
    return memory[memoryAddress];
}


/*
 * Returns the word located at <memoryAddress> in memory.
 */
unsigned long Memory::readWord(unsigned long memoryAddress)
{        
    unsigned long word, hexDigit;
    
    /* Reads four bytes one by one starting from lowest to highest. Once a byte is read, it is left shifted
     * by 24 bits followed by right shifted by 24 bits to clear higher order 24 bits, if set by sign extension 
     * caused by widening of data during auto-casting. Casting takes place because of hexDigit being an
     * unsigned long (32 bits) while readByte() returns data of type char (8 bits). All four bytes are
     * packed together to form a 32 bit word.
     */
    hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = hexDigit;
    hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
    hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
    hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
    
    return word;
}

int Memory::writeWord(unsigned long memoryAddress, unsigned long word)
{ 
    char byte;
    byte = (word & 0xFF000000) >> 24;               // Write the first byte.
    writeByte(memoryAddress++, byte);
    byte = (word & 0x00FF0000) >> 16;               // Write the second byte.
    writeByte(memoryAddress++, byte);
    byte = (word & 0x0000FF00) >> 8;                // Write the third byte.
    writeByte(memoryAddress++, byte);
    byte = word & 0x000000FF;                       // Write the fourth byte.
    writeByte(memoryAddress, byte);
       
	return RET_SUCCESS;
}


/*
 * Writes the Half Word located at <memoryAddress> in memory with <halfWord>.
 */
int Memory::writeHalfWord(unsigned long memoryAddress, unsigned short halfWord)
{
    char byte;
    byte = (halfWord & 0xFF00) >> 8;            // Write the first byte.
    writeByte(memoryAddress++, byte);
    byte = halfWord & 0x00FF;                   // Write the second byte.
    writeByte(memoryAddress, byte);
        
	return RET_SUCCESS;
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
