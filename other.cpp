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


// Toggles a single bit at a specified position of a 32-bit bitStream
unsigned long Other::toggleBit(unsigned long bitStream, int position)
{
	unsigned long toggledBitStream = bitStream;
	toggledBitStream ^= 1 << position;
	return toggledBitStream;
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



/*
 * Returns 1, if memoryAddress is not aligned on <alignment> boundary.
 * Returns 0, if memoryAddress is aligned on <alignment> boundary.
 * It's useful to prevent loading mis-aligned memoryAddress
 * in double word (DWORD) instructions by raising
 * memory_address_not_aligned traps.
 */
int Other::is_mem_address_not_aligned(unsigned long memoryAddress, int alignment)
{
    switch(alignment)
    {
        case 2: if((((unsigned long)(memoryAddress / 2)) * 2) == memoryAddress) return 0; else return 1;
        case 4: if((((unsigned long)(memoryAddress / 4)) * 4) == memoryAddress) return 0; else return 1;
        case 8: if((((unsigned long)(memoryAddress / 8)) * 8) == memoryAddress) return 0; else return 1;
    }
    return 0;
}



/*
 * Returns 1, if sparcRegister is mis-aligned (Odd).
 * Returns 0, if sparcRegister is not mis-aligned (Even).
 * It's useful to prevent loading mis-aligned registers
 * in double word (DWORD) instructions by raising
 * illegal_instruction and invalid_fp_register traps.
 */
int Other::is_register_mis_aligned(char* sparcRegister)
{
    unsigned short registerIndex;
    
    registerIndex = sparcRegister[1] - '0';             // First digit is taken out as integer, e.g. '2' from %f23
    
    /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
     * e.g. %f23, firstDigit = 2, secondDigit = 3
     * registerIndex = (2 * 10 + 3) = 23
     */
    if(sparcRegister[2] != '\0')
        registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
    
    // Is register mis-aligned?
    if(registerIndex % 2)
        return 1;       // Odd register
    else
        return 0;       // Even register
}


/*
 * Returns 1, if the 32-bit floating point number is Sounding NaN(SNaN), 
 * Returns 0, otherwise.
 */
int Other::isSNaN32(float float32)
{
	convertFloat.hexToFloat = float32;
	int exponent = (convertFloat.floatToHex & 0x7F800000) >> 23;
	int mantissa = convertFloat.floatToHex & 0x007FFFFF;
	int msb_mantissa = (convertFloat.floatToHex & 0x00400000) >> 22;
	if((exponent == 255 ) && mantissa && !msb_mantissa)
		return 1;
	else
		return 0;
}



/*
 * Returns 1, if the 32-bit floating point number is Quiet NaN(QNaN), 
 * Returns 0, otherwise.
 */
int Other::isQNaN32(float float32)
{
	if(isnan(float32) && !isSNaN32(float32))
		return 1;
	else
		return 0;
}



/*
 * Returns 1, if the 64-bit floating point number is Sounding NaN(SNaN), 
 * Returns 0, otherwise.
 */
int Other::isSNaN64(double float64)
{
	convertDouble.hexToDouble = float64;
	int exponent = (convertDouble.doubleToHex[0] & 0x7FF00000) >> 20;
	int mantissa_high = convertDouble.doubleToHex[0] & 0x000FFFFF;
	int mantissa_low = convertDouble.doubleToHex[1];
	int msb_mantissa = (convertDouble.doubleToHex[0] & 0x00080000) >> 19;
	if((exponent == 2047) && (mantissa_high || mantissa_low) && !msb_mantissa)
		return 1;
	else
		return 0;
}



/*
 * Returns 1, if the 64-bit floating point number is Quiet NaN(QNaN), 
 * Returns 0, otherwise.
 */
int Other::isQNaN64(double float64)
{
	if(isnan(float64) && !isSNaN64(float64))
		return 1;
	else
		return 0;
}




