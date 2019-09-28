#include "register.h"

Other *other_r = new Other();

void Register::initializeRegisters()
{
	unsigned short count;
	
	if(SRegisters.registerSet != NULL)
		free(SRegisters.registerSet);
	if(SRegisters.globalRegisters != NULL)
		free(SRegisters.globalRegisters);
	SRegisters.registerWindows = REGISTER_WINDOWS;       // get number of register windows from configuration file
        
    /* SRegisters.registerSet holds the base address of the set of register windows.
     * Each register window consists of a set of IN and LOCAL registers of its own, as well as
     * a set of OUT registers shared from its adjacent window. Hence, there are (IN + LOCAL) = (8 + 8) = 16
     * registers in a window (REGISTER_WINDOW_WIDTH), each register being 4 byte (SIZEOF_INTEGER_REGISTER)
     * wide. Therefore, 16 * 8 * <Number of register windows> bytes has been allocated.
     */
	SRegisters.registerSet = (unsigned long*)malloc(REGISTER_WINDOW_WIDTH * REGISTER_WINDOWS * SIZEOF_INTEGER_REGISTER);
    //	SRegisters.registerSet = (unsigned long*)malloc(sizeof(unsigned long)*512);

    //	SRegisters.registerSet = new unsigned long*[REGISTER_WINDOW_WIDTH * REGISTER_WINDOWS * SIZEOF_INTEGER_REGISTER];
        
    // GLOBAL registers, not being part of of register window, are contained in memory pointed to by SRegisters.globalRegisters.
	SRegisters.globalRegisters = (unsigned long*)malloc(SIZEOF_INTEGER_REGISTER * GLOBAL_REGISTERS);
    //	SRegisters.globalRegisters = (unsigned long*)malloc(sizeof(unsigned long)*32);

    //	SRegisters.globalRegisters = new unsigned long*[SIZEOF_INTEGER_REGISTER * GLOBAL_REGISTERS];
        
    /* SRegisters.cwptr will always point to the base address of the current register window.
     * It is initialized by the base address of set of register windows, i.e. SRegisters.cwptr
     */
	SRegisters.cwptr = SRegisters.registerSet;

	// Initialize psr
	SRegisters.psr.setCwp(0);
	SRegisters.psr.setEt(0);
	SRegisters.psr.setPs(0);
	SRegisters.psr.setS(1);
	SRegisters.psr.setPil(0);
	SRegisters.psr.setEf(0);
	SRegisters.psr.setEc(0);
	SRegisters.psr.setReserved(0);
	
	SRegisters.psr.setC(0);
	SRegisters.psr.setV(0);
	SRegisters.psr.setZ(0);
	SRegisters.psr.setN(0);
	SRegisters.psr.setVer(3);
	SRegisters.psr.setImpl(0xF);
        
    // Initialize fsr
    SRegisters.fsr.setCexc(0);
    SRegisters.fsr.setAexc(0);
    SRegisters.fsr.setFcc(0);
    SRegisters.fsr.setUlow(0);
    SRegisters.fsr.setQne(0);
    SRegisters.fsr.setFtt(0);
    SRegisters.fsr.setVer(0);
    SRegisters.fsr.setRes(0);
    SRegisters.fsr.setNs(0);        
    SRegisters.fsr.setTem(0);
    SRegisters.fsr.setUhigh(0);
    SRegisters.fsr.setRd(0);

	// Initialize wim, tbr, y, pc, npc
	SRegisters.wim = 0;
	SRegisters.tbr = 0;
	SRegisters.y = 0;
	SRegisters.pc = 0;
	SRegisters.npc = 4;                 // nPC should point to the instruction ahead.

	// Initialize global registers
	for(count = 0; count < GLOBAL_REGISTERS; count++)
	SRegisters.globalRegisters[count] = 0;

	// Initialize out, local, in registers
	for(count = 0; count < REGISTER_WINDOW_WIDTH * REGISTER_WINDOWS; count++)
	SRegisters.registerSet[count] = 0;
        
    // Initialize ASR
	for(count = 0; count < 32; count++)
	SRegisters.asrRegisters[count] = 0;
        
    // Initialize floating point registers
	for(count = 0; count < 32; count++)
	SRegisters.floatingPointRegisters[count] = 0;
}


/*
 * Displays 32-bit value contained in a register in HEX format, 
 * prepended with zeroes used for padding if the value is shorter than 32 bit.
 */
char* Register::displayRegister(unsigned long registerValue)
{
    char* hexNumber = (char*)malloc(11);
    sprintf(hexNumber, "0x%08lX", registerValue);
    return hexNumber;
}

/*
 * Returns the current window pointer value contained in CWP field of PSR.
 */
unsigned short Register::getRegisterWindow()
{
    return SRegisters.psr.getCwp();
}

/*
 * Sets the current window pointer value contained in CWP field of PSR.
 * CWP, being a field 5 bit wide, can vary in the range [0-32]. Specific 
 * implementation can cut the maximum limit short by specifying REGISTER_WINDOWS
 * token in configuration file (sparcsim.conf).
 * Also adjusts current window pointer address (SRegisters.cwptr) 
 * held in memory. SRegisters.registerSet is the base address of register 
 * set and CWP multiplied by REGISTER_WINDOW_WIDTH gives the offset.
 */
void Register::setRegisterWindow(unsigned short registerWindow)
{
    SRegisters.psr.setCwp(registerWindow);
    SRegisters.cwptr = SRegisters.registerSet + SRegisters.psr.getCwp() * REGISTER_WINDOW_WIDTH;
}


/*
 * Returns the pointer to current register window forward or backward
 * as specified by direction. Positive direction indicates
 * forward move and negative direction indicates backward move.
 * Wrap around is done, if needed.
 * SAVE and RESTORE SPARC instructions require the pointer to
 * register window to be shifted appropriately.
 */
unsigned long* Register::getWindowPointer(int direction)
{
   // Move window pointer forward
   if(direction == 1)
    {
        if(SRegisters.psr.getCwp() == (SRegisters.registerWindows - 1))
            return SRegisters.registerSet;
        else
            return SRegisters.cwptr + REGISTER_WINDOW_WIDTH;
    }
	
    // Move window pointer backward
    else
    {
        if(SRegisters.psr.getCwp() == 0)
            return SRegisters.registerSet + (REGISTER_WINDOW_WIDTH * (SRegisters.registerWindows - 1));
        else
            return SRegisters.cwptr - REGISTER_WINDOW_WIDTH;
    }
}



unsigned long Register::getRegister(char* sparcRegister)
{
	char registerType, SRegisterstring[6];
	unsigned short registerIndex, charIndex = 0;
	unsigned long* previousWindowPointer = getWindowPointer(-1);            
	// Previous window pointer will be useful to access OUT registers shared from previous adjacent window. 
	
    /* 
     Register name is copied to a temporary variable to avoid segmentation fault.
     gcc core dump analysis shows that it faults while trying to modify a string literal
     from an invocation like setRegister("%o7", registerValue) from CALL type instruction 
    */
    strcpy(SRegisterstring, sparcRegister); 
    sparcRegister = SRegisterstring;
    
    // Strip off leading % symbol, if present
    if(sparcRegister[0] == '%')
    {
        while(sparcRegister[charIndex++])
            sparcRegister[charIndex - 1] = sparcRegister[charIndex];
        sparcRegister[charIndex - 2] = '\0'; 
    }
        
	if(!(strcmp(sparcRegister, "g0")))
        return 0;                       // %g0 will always contain zero, by definition.

	if(!strcmp(sparcRegister, "psr")) 
        return castPSRToUnsignedLong(SRegisters.psr);       // Cast PSR structure to 4 byte long integer and return.
        
    if(!strcmp(sparcRegister, "fsr")) 
        return castFSRToUnsignedLong(SRegisters.fsr);       // Cast FSR structure to 4 byte long integer and return.
		
	if(!strcmp(sparcRegister, "wim"))
		return SRegisters.wim;
	
	if(!strcmp(sparcRegister, "tbr"))
		return SRegisters.tbr;
	
	if(!strcmp(sparcRegister, "y"))
		return SRegisters.y;
		
	if(!strcmp(sparcRegister, "pc"))
		return SRegisters.pc;
		
	if(!strcmp(sparcRegister, "npc"))
		return SRegisters.npc;
        
    if((sparcRegister[0] == 'a') && (sparcRegister[1] == 's') && (sparcRegister[2] == 'r'))
    {
        char* asrRegister = (char*)malloc(3);
        charIndex = 0;
        if(sparcRegister[3] >= '0' && sparcRegister[3] <= '9')
            asrRegister[charIndex++] = sparcRegister[3];            
			// If 4th character is a digit, copy it to asrRegister, i.e. 2 is taken out from %asr23.
        if(sparcRegister[4] >= '0' && sparcRegister[4] <= '9')
            asrRegister[charIndex++] = sparcRegister[4];            
			// If 5th character is a digit, copy it to asrRegister, i.e. 3 is taken out from %asr23.
        asrRegister[charIndex] = '\0';        
        registerIndex = strtoul(asrRegister, NULL, 0);              
		// Convert asrRegister to integer index, i.e. 23 is obtained from %asr23.
        return SRegisters.asrRegisters[registerIndex]; 
    }    
    registerType = sparcRegister[0];                // First character will indicate type of register, i.e. global(g), local(l), in(i), out(o)
	registerIndex = sparcRegister[1] - '0';         // Integer register index will be a single digit ranging between [0-7]
        
    /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
     * e.g. %f23, firstDigit = 2, secondDigit = 3
     * registerIndex = (2 * 10 + 3) = 23
     */
    if(sparcRegister[2] != '\0')                    
        registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
	switch (registerType)
	{
	case 'o':
		return *(previousWindowPointer + registerIndex);                // OUT register are shared with IN registers from previous window.
	case 'l':
		return *(SRegisters.cwptr + 8 + registerIndex);                 // In a register window, LOCAL registers follow IN registers.
	case 'i':
		return *(SRegisters.cwptr + registerIndex);                     // In a register window, IN registers precede LOCAL registers.
	case 'g':
		return *(SRegisters.globalRegisters + registerIndex);           // GLOBAL registers are allocated separately from register window.
    case 'f':
        return SRegisters.floatingPointRegisters[registerIndex];    // FP registers are stored in an array.
	}
	return 0;
}



/*
 * Stores a 32-bit value to a SPARC register.
 */
int Register::setRegister(char* sparcRegister, unsigned long registerValue)
{
	char registerType, SRegisterstring[6];
	unsigned short registerIndex, charIndex = 0;
	unsigned long* previousWindowPointer = getWindowPointer(-1);            
	// Previous window pointer will be useful to access OUT registers shared from previous adjacent window. 

    /* 
     Register name is copied to a temporary variable to avoid segmentation fault.
     Program core dump analysis shows that it faults while trying to modify a string
     literal from an invocation like setRegister("%o7", registerValue) from CALL instruction 
    */
    strcpy(SRegisterstring, sparcRegister); 
    sparcRegister = SRegisterstring;
    
    // Strip off leading % symbol, if present
    if(sparcRegister[0] == '%')
    {
        while(sparcRegister[charIndex++])
            sparcRegister[charIndex - 1] = sparcRegister[charIndex];
        sparcRegister[charIndex - 2] = '\0'; 
    }
    
    if(!(strcmp(sparcRegister, "g0")))
		return RET_SUCCESS;             // %g0 always contains zero, by definition.

	if(!strcmp(sparcRegister, "psr"))
	{
        if(!SRegisters.psr.getS())
        {
            printf("Attempt to write PSR register from user mode");
            return RET_TRAP;
        }
        
        if((registerValue & 0x0000001F /* Extract CWP field from PSR */) >= SRegisters.registerWindows)
        {
            printf("Attempt to set CWP >= NWINDOWS");
            return RET_TRAP;
        }
        SRegisters.psr = castUnsignedLongToPSR(registerValue);      // Cast 4 byte long integer to PSR structure.
        SRegisters.cwptr = SRegisters.registerSet + SRegisters.psr.getCwp() * REGISTER_WINDOW_WIDTH;     
		// Writing to PSR's CWP field requires pointer to current register window (cwptr) to be adjusted, too, at the same time.
        return RET_SUCCESS;
	}
        
    if(!strcmp(sparcRegister, "fsr"))
    {
            SRegisters.fsr = castUnsignedLongToFSR(registerValue);      // Cast 4 byte long integer to FSR structure.
            return RET_SUCCESS;;
    }
		
	if(!strcmp(sparcRegister, "wim"))
    {
		if(!SRegisters.psr.getS())
		{
		    printf("Attempt to write WIM register from user mode");
		    return RET_TRAP;
		}
		SRegisters.wim = registerValue;
		return RET_SUCCESS;
    }

	if(!strcmp(sparcRegister, "tbr"))
    {
		if(!SRegisters.psr.getS())
   	    {
            printf("Attempt to write TBR register from user mode");
            return RET_TRAP;
        }
        SRegisters.tbr = registerValue & 0xFFFFF000;                // TBR's lower 12 bits are read-only.
        return RET_SUCCESS;
    }

	if(!strcmp(sparcRegister, "y"))
    {
		SRegisters.y = registerValue;
        return RET_SUCCESS;
    }

	if(!strcmp(sparcRegister, "pc"))
    {
		SRegisters.pc = registerValue;
        return RET_SUCCESS;
    }

	if(!strcmp(sparcRegister, "npc"))
    {
		SRegisters.npc = registerValue;
        return RET_SUCCESS;
    }
    
    if((sparcRegister[0] == 'a') && (sparcRegister[1] == 's') && (sparcRegister[2] == 'r'))
    {
        char* asrRegister = (char*)malloc(3);
        charIndex = 0;
        if(sparcRegister[3] >= '0' && sparcRegister[3] <= '9')
            asrRegister[charIndex++] = sparcRegister[3];            
			// If 4th character is a digit, copy it to asrRegister, i.e. 2 is taken out from %asr23.
        if(sparcRegister[4] >= '0' && sparcRegister[4] <= '9')
            asrRegister[charIndex++] = sparcRegister[4];            
			// If 4th character is a digit, copy it to asrRegister, i.e. 3 is taken out from %asr23.
        asrRegister[charIndex] = '\0';
        
        registerIndex = strtoul(asrRegister, NULL, 0);              
		// Convert asrRegister to integer index, i.e. 23 is obtained from %asr23.
        SRegisters.asrRegisters[registerIndex] = registerValue; 
        return RET_SUCCESS;
    }

        
    registerType = sparcRegister[0];                // First character will indicate type of register, i.e. global(g), local(l), in(i), out(o)
	registerIndex = sparcRegister[1] - '0';         // Integer register index will be a single digit ranging between [0-7]
        
    /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
     * e.g. %f23, firstDigit = 2, secondDigit = 3
     * registerIndex = (2 * 10 + 3) = 23
     */
    if(sparcRegister[2] != '\0')
        registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');

	switch (registerType)
	{
		case 'o':
			*(previousWindowPointer + registerIndex) = registerValue;       
			// OUT register are shared with IN registers from previous window.
			return RET_SUCCESS;
		case 'l':
			*(SRegisters.cwptr + 8 + registerIndex) = registerValue;    
			// In a register window, LOCAL registers follow IN registers.
			return RET_SUCCESS;
		case 'i':
			*(SRegisters.cwptr + registerIndex) = registerValue;        
			// In a register window, IN registers precede LOCAL registers.
			return RET_SUCCESS;
		case 'g':
			*(SRegisters.globalRegisters + registerIndex) = registerValue;      
			// GLOBAL registers are allocated separately from register window.
			return RET_SUCCESS;
		case 'f':
			SRegisters.floatingPointRegisters[registerIndex]= registerValue;    
			// FP registers are stored in an array.
			return RET_SUCCESS;
	}
    return RET_FAILURE;
}



/*
 * Returns PSR register's content as 32-bit integer value.
 */

processor_status_register Register::getPSR()
{
    return SRegisters.psr;
}



/*
 * Sets PSR register's fields as decoded from a 32-bit integer value.
 */
 
/* 
void Register::setPSR(processor_status_register psrValue)
{
    SRegisters.psr = castUnsignedLongToPSR(psrValue);
}
*/


/*
 * Returns FSR register's content as 32-bit integer value.
 */
floating_point_state_register Register::getFSR()
{
    return SRegisters.fsr;
}



/*
 * Sets FSR register's fields as decoded from a 32-bit integer value.
 */
 
/* 
void Register::setFSR(floating_point_state_register fsrValue)
{
    SRegisters.fsr = castUnsignedLongToFSR(fsrValue);
}
*/


/*
 * Returns TBR register's content as 32-bit integer value.
 */
unsigned long Register::getTBR()
{
    return SRegisters.tbr;
}



/*
 * To set tbr.tt field from trap.c: setTrapCode()for an instruction which, itself, attempts to 
 * write TBR in user mode, setTBR() has to be used instead of setRegister("tbr", registerValue)
 * Because, setRegister() checks whether the processor is in user/supervisor mode. If it is the
 * reason for the privileged_instruction trap caused by original instruction, the execution
 * comes back at the same point from where it was initiated. Hence, the result is an infinite loop.
 */

void Register::setTBR(unsigned long tbrValue)
{
    SRegisters.tbr = tbrValue;
}



/*
 * Performs actions associated with SAVE instruction.
 */
int Register::saveRegisters()
{
	unsigned long regPSR, regWIM;
	struct processor_status_register psr;
	short nextCWP;

	regPSR = getRegister("psr");
    psr = castUnsignedLongToPSR(regPSR);
	regWIM = getRegister("wim");

	if(psr.getCwp() == 0)
		nextCWP = SRegisters.registerWindows - 1;           // Wrap around when CWP = 0
	else
		nextCWP = SRegisters.psr.getCwp() - 1;                   // Decrease CWP if non-zero.

    // Test for window overflow.
	if(other_r->getBit(regWIM, nextCWP))
	{
		printf("Register window overflow has occurred");
		return RET_TRAP;
	}
    // If window_overflow does not take place.
	else
	{
		SRegisters.cwptr = getWindowPointer(-1);            // Decrease pointer to current register window.
        SRegisters.psr.setCwp(nextCWP);                       // Update CWP.
		return RET_SUCCESS;
	}
}



/*
 * Performs actions associated with RESTORE instruction.
 */
int Register::restoreRegisters()
{
	unsigned long regPSR, regWIM;
	struct processor_status_register psr;
	short nextCWP;

	regPSR = getRegister("psr");
    psr = castUnsignedLongToPSR(regPSR);
	regWIM = getRegister("wim");

	if(psr.getCwp() == (SRegisters.registerWindows - 1))
		nextCWP = 0;                                            // Wrap around when CWP = <Number of register windows>.
	else
		nextCWP = SRegisters.psr.getCwp() + 1;                   // Increase CWP if not equal to <Number of register windows>.


    // Test for window underflow.
	if(other_r->getBit(regWIM, nextCWP))
	{
		printf("Register window underflow has occurred");
		return RET_TRAP;
	}
        
    // If window_underflow does not take place.
	else
	{
		SRegisters.cwptr = getWindowPointer(1);               // Increase pointer to current register window.
        SRegisters.psr.setCwp(nextCWP);                       // Update CWP.
		return RET_SUCCESS;
	}
}



/*
 * Decreases psr.getCwp() by 1, wraps around if needed.
 * Also adjusts pointer to current register window (cwptr)
 */
void Register::nextWindow()
{
	if(SRegisters.psr.getCwp() == 0)
		SRegisters.psr.setCwp(SRegisters.registerWindows - 1);
	else
		SRegisters.psr.setCwp(SRegisters.psr.getCwp() - 1);
    SRegisters.cwptr = getWindowPointer(-1);
}



/*
 * Unpacks PSR fields from a 32-bit value and assigns 
 * to relevant members of PSR structure. Bit masking has
 * been done according to PSR field definition as
 * specified by SPARC v8 manual. Returns an instance
 * of PSR structure filled with supplied information.
 */
processor_status_register Register::castUnsignedLongToPSR(unsigned long registerValue)
{
	processor_status_register psr;

	psr.setCwp(registerValue & 0x00000001F);
	psr.setEt((registerValue & 0x00000020) >> 5);
	psr.setPs((registerValue & 0x00000040) >> 6);
	psr.setS((registerValue & 0x00000080) >> 7);
	psr.setPil((registerValue & 0x00000F00) >> 8);
	psr.setEf((registerValue & 0x00001000) >> 12);
	psr.setEc((registerValue & 0x00002000) >> 10);
	psr.setReserved((registerValue & 0x000FC000) >> 14);
	psr.setC((registerValue & 0x00100000) >> 20);
	psr.setV((registerValue & 0x00200000) >> 21);
	psr.setZ((registerValue & 0x00400000) >> 22);
	psr.setN((registerValue & 0x00800000) >> 23);
	psr.setVer((registerValue & 0x0F000000) >> 24);
	psr.setImpl((registerValue & 0xF0000000) >> 28);

	return psr;
}


/*
 * Packs PSR fields to a 32-bit value. Bit shifting has
 * been done according to PSR field definition as
 * specified by SPARC v8 manual.
 */
unsigned long Register::castPSRToUnsignedLong(processor_status_register psr)
{
	unsigned long registerValue = 0;

	registerValue = registerValue | psr.getCwp();
	registerValue = registerValue | (psr.getEt() << 5);
	registerValue = registerValue | (psr.getPs() << 6);
	registerValue = registerValue | (psr.getS() << 7);
	registerValue = registerValue | (psr.getPil() << 8);
	registerValue = registerValue | (psr.getEf() << 12);
	registerValue = registerValue | (psr.getEc() << 13);
	registerValue = registerValue | (psr.getReserved() << 14);
	registerValue = registerValue | (psr.getC() << 20);
	registerValue = registerValue | (psr.getV() << 21);
	registerValue = registerValue | (psr.getZ() << 22);
	registerValue = registerValue | (psr.getN() << 23);
	registerValue = registerValue | (psr.getVer() << 24);
	registerValue = registerValue | (psr.getImpl() << 28);	

	return registerValue;
}


/*
 * Unpacks FSR fields from a 32-bit value and assigns 
 * to relevant members of FSR structure. Bit masking has
 * been done according to FSR field definition as
 * specified by SPARC v8 manual. Returns an instance
 * of FSR structure filled with supplied information.
 */
floating_point_state_register Register::castUnsignedLongToFSR(unsigned long registerValue)
{
	floating_point_state_register fsr;
        
    fsr.setCexc(registerValue & 0x0000001F);
    fsr.setAexc((registerValue & 0x000003E0) >> 5);
    fsr.setFcc((registerValue & 0x00000C00) >> 10);
    fsr.setUlow((registerValue & 0x00001000) >> 12);
    fsr.setQne((registerValue & 0x00002000) >> 13);
    fsr.setFtt((registerValue & 0x0001C000) >> 14);
    fsr.setVer((registerValue & 0x000E0000) >> 17);
    fsr.setRes((registerValue & 0x00300000) >> 20);
    fsr.setNs((registerValue & 0x00400000) >> 22);
    fsr.setTem((registerValue & 0x0F800000) >> 23);
    fsr.setUhigh((registerValue & 0x30000000) >> 28);
    fsr.setRd((registerValue & 0xC0000000) >> 30);

	return fsr;
}



/*
 * Packs FSR fields to a 32-bit value. Bit shifting has
 * been done according to FSR field definition as
 * specified by SPARC v8 manual.
 */
unsigned long Register::castFSRToUnsignedLong(floating_point_state_register fsr)
{
	unsigned long registerValue = 0;

	registerValue = registerValue | fsr.getCexc();
	registerValue = registerValue | (fsr.getAexc() << 5);
	registerValue = registerValue | (fsr.getFcc() << 10);
	registerValue = registerValue | (fsr.getUlow() << 12);
	registerValue = registerValue | (fsr.getQne() << 13);
	registerValue = registerValue | (fsr.getFtt() << 14);
	registerValue = registerValue | (fsr.getVer() << 17);
	registerValue = registerValue | (fsr.getRes() << 20);
	registerValue = registerValue | (fsr.getNs() << 22);
	registerValue = registerValue | (fsr.getTem() << 23);
	registerValue = registerValue | (fsr.getUhigh() << 28);
	registerValue = registerValue | (fsr.getRd() << 30);	

	return registerValue;
}



/*
 * Returns next register in a set of registers of similar type.
 * e.g. If %f24 is passed as argument, %f25 is returned.
 * It finds use in implementing double word (DWORD) opcodes.
 */
char* Register::getNextRegister(char* sparcRegister)
{
    char nextRegister[15];
    /*
     *  In earlier implementation, nextRegister[6] was declared. When a garbage, i.e. a  string NOT
     *  representing a register was being passed from execute.c (convertDouble.doubleToHex[1] = getRegister(getNextRegister(tokens[1])))
     *  e.g. 0xFFFFFF90 (length = 10), it was passing out of array boundary.
     */
    unsigned short nextRegisterIndex, characterCount = 2;               // Because register name will be at least two character long, e.g. g5
    
    strcpy(nextRegister, sparcRegister);                // Copy the register name to prevent alteration of actual register
    sparcRegister = nextRegister;
    
    nextRegisterIndex = sparcRegister[1] - '0';         // First digit is taken out as integer, e.g. '2' from %f23
    
    /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
     * e.g. %f23, firstDigit = 2, secondDigit = 3
     * registerIndex = (2 * 10 + 3) = 23
     */
    if(sparcRegister[2] != '\0')
        nextRegisterIndex = nextRegisterIndex * 10 +  (sparcRegister[2] - '0');
    
    nextRegisterIndex++;                                // Next register is calculated, e.g. 25 for 24.
    sparcRegister[1] = nextRegisterIndex / 10 + '0';    // 1's digit converted to character.
    
    // If a 10's digit is present, it is converted to character.
    if(nextRegisterIndex % 10 != 0)
    {
        sparcRegister[characterCount] = nextRegisterIndex % 10 + '0';
        characterCount++;
    }
    sparcRegister[characterCount] = '\0';
    
    return sparcRegister;
}


/*
 * Translates the effective address for LOAD/STORE instructions
 * and returns the translated address as a string,
 * e.g. '%g3 + 0x25' or '%l2 + %l3'.
 * 
 * The effective address for a load/store instruction is 
 * r[rs1] + r[rs2], if i = 0, or r[rs1] + sign_ext(simm13), if i = 1.
 * The 'i' bit selects the second ALU operand for (integer) arithmetic and
 * load/store instructions. If i = 0, the operand is r[rs2]. If i = 1, 
 * the operand is 'simm13', sign-extended from 13 to 32 bits.
 */
char* Register::getAddress(unsigned long rs1, unsigned long rs2, unsigned long i, unsigned long simm13, int registerTypeIdentifier)
{
	char* address = (char*)malloc(30);
	char* hexNumber = (char*)malloc(32);
	address[0] = '\0';
	
    // Integer, Floating-point or Co-Processor register?
    switch(registerTypeIdentifier)
    {
        case 1: strcat(address, getIntegerRegisterName(rs1)); break;           // Integer register.
        case 2: strcat(address, getFloatingRegisterName(rs1)); break;          // Floating point register.
        case 3: strcat(address, getCoProcessorRegisterName(rs1)); break;       // Co-Processor register.
    }

    // Second operand is rs2.
	if(i == 0) 
	{
		strcat(address, " + ");
		switch(registerTypeIdentifier)
		{
	        case 1: strcat(address, getIntegerRegisterName(rs2)); break;            // Integer register.
	        case 2: strcat(address, getFloatingRegisterName(rs2)); break;           // Floating point register.
	        case 3: strcat(address, getCoProcessorRegisterName(rs2)); break;        // Co-Processor register.
		}

	}    
    // Second operand is simm13.
	else
	{
		strcat(address, " + ");
		sprintf(hexNumber, "0x%lX", simm13);
		strcat(address, hexNumber);
	}
	
	free(hexNumber);
	return address;
}



/*
 * Returns the second operand of a LOAD/STORE instruction as a string.
 * 
 * The 'i' bit selects the second ALU operand for (integer) arithmetic and
 * load/store instructions. If i = 0, the operand is r[rs2]. If i = 1, 
 * the operand is 'simm13', sign-extended from 13 to 32 bits.
 */
char* Register::getReg_Or_Imm(unsigned long rs2, unsigned long i, unsigned long simm13, int registerTypeIdentifier)
{
	char* address = (char*)malloc(30);
	char* hexNumber = (char*)malloc(32);
	
        // Second operand is rs2.
	if(i == 0) 
	{
		// Integer, Floating-point or Co-Processor register?
		switch(registerTypeIdentifier)
		{
			case 1: strcpy(address, getIntegerRegisterName(rs2)); break;       // Integer register.
			case 2: strcpy(address, getFloatingRegisterName(rs2)); break;      // Floating point register.
			case 3: strcpy(address, getCoProcessorRegisterName(rs2)); break;   // Co-Processor register.
		}
	}    
    // Second operand is simm13.
	else
	{
		sprintf(hexNumber, "0x%lX", simm13);
		strcpy(address, hexNumber);
	}
	free(hexNumber);
	return address;
}



/*
 * Returns the mnemonic name of an Integer register as string
 * (e.g. %g6, %l7) taking 'registerIdentifier' (e.g. 6, 7) as argument.
 * Translation is done according to the specification given in
 * Chapter - 4 (Registers) of SPARC v8 manual.
 * 
 * in[0] - in[7]         => r[24] - r[31]
 * local[0] - local[7]   => r[16] - r[23]
 * out[0] - out[7]       => r[8] - r[15]
 * global[0] - global[7] => r[0] - r[7]
 */
char* Register::getIntegerRegisterName(unsigned long registerIdentifier)
{
	char* registerName = (char*) malloc(4);
	char* registerIndex = (char*)malloc(2);
	
	if((registerIdentifier >= 0) && (registerIdentifier <=7))
		strcpy(registerName, "%g");
	else
		if((registerIdentifier >= 8) && (registerIdentifier <=15))
			strcpy(registerName, "%o");
		else
			if((registerIdentifier >= 16) && (registerIdentifier <=23))
				strcpy(registerName, "%l");
			else
				strcpy(registerName, "%i");
	
	sprintf(registerIndex, "%ld", (registerIdentifier % 8));
	strcat(registerName, registerIndex);
	
	free(registerIndex);
	return registerName;
}



/*
 * Returns the mnemonic name of a Floating-point register as string
 * (e.g. %f31) taking 'registerIdentifier' (e.g. 31) as argument.
 */
char* Register::getFloatingRegisterName(unsigned long registerIdentifier)
{
	char* registerName = (char*) malloc(4);
	char* registerIndex = (char*)malloc(3);
	
	strcpy(registerName, "%f");
	sprintf(registerIndex, "%ld", registerIdentifier);
	strcat(registerName, registerIndex);
	
	free(registerIndex);
	return registerName;
}



/*
 * Returns the mnemonic name of a Co-Processor register as string
 * (e.g. %f31) taking 'registerIdentifier' (e.g. 31) as argument.
 */
char* Register::getCoProcessorRegisterName(unsigned long registerIdentifier)
{
	char* registerName = (char*) malloc(4);
	char* registerIndex = (char*)malloc(3);
	
	strcpy(registerName, "%f");
	sprintf(registerIndex, "%ld", registerIdentifier);
	strcat(registerName, registerIndex);
	
	free(registerIndex);
	return registerName;
}

