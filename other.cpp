#include "other.h"
// Returns a single bit at a specified position of a 32-bit bitStream
int Other::getBit(unsigned long bitStream, int position)
{
	int bit;
	bit = (bitStream & (1 << position)) >> position;
	return bit;
}



// Sets a single bit at a specified position of a 32-bit bitStream
unsigned long Other::setBit(unsigned long bitStream, int position)
{
	unsigned long setBitStream = bitStream;
	setBitStream |= 1 << position;
	return setBitStream;
}



// Clears a single bit at a specified position of a 32-bit bitStream
unsigned long Other::clearBit(unsigned long bitStream, int position)
{
	unsigned long clearedBitStream = bitStream;
	clearedBitStream &= ~(1 << position);
	return clearedBitStream;
}



// Toggles a single bit at a specified position of a 32-bit bitStream
unsigned long Other::toggleBit(unsigned long bitStream, int position)
{
	unsigned long toggledBitStream = bitStream;
	toggledBitStream ^= 1 << position;
	return toggledBitStream;
}



// Returns a pointer to a string containing bytes within startPosition and endPosition extracted from 32-bit bitStream
char* Other::showBits(unsigned long bitStream, int startPosition, int endPosition)
{
	// Allocate one extra byte for NULL character
	char* bits = (char*)malloc(endPosition - startPosition + 2);
	int bitIndex;
	for(bitIndex = 0; bitIndex <= endPosition; bitIndex++)
		bits[bitIndex] = (getBit(bitStream, endPosition - bitIndex)) ? '1' : '0';
	bits[bitIndex] = '\0';
	return bits;
}




void Other::displayWord(char* cpuInstruction, int isInstruction)
{
	if(cpuInstruction != NULL)
	{
		int count; unsigned int hexDigit; 
		for(count = 0; count <= 3; count++)
		{
			char instructionByte = cpuInstruction[count];
			hexDigit = instructionByte;
                        
                       /* hexDigit is left shifted by 24 bits followed by right shifted by 24 bits 
                        * to clear higher order 24 bits, if set by sign extension caused by widening 
                        * of data during auto-casting. Casting takes place because of hexDigit being an
                        * unsigned long (32 bits) while cpuInstruction is an array of type char (8 bits).
                        */
			hexDigit = (hexDigit << 24) >> 24;
			printf("%02X", hexDigit);
			if(isInstruction)
				printf(" ");
		}
	}
}

