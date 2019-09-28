#include <execute.h>


/*
 * Given an instruction disassembled in assembly, executes it.
 */
 
Other *other = new Other();

int Execute::executeInstruction(char* disassembledInstruction, Memory *memory, Register *sregister)
{
	char tokens[10][20];
	char* token;
	short count, isFormatIIIOpcodeFound;
    unsigned short index;
	unsigned long memoryAddress, regPC, regnPC, regPSR, regFSR, regRS1, regRS2, reg_or_imm, regRD;
    float float_regRS1, float_regRS2, float_regRD;
    double double_regRS1, double_regRS2, double_regRD;
	
	count = 0;
	isFormatIIIOpcodeFound = -1;
	regPC = sregister->getRegister("pc");
	regnPC = sregister->getRegister("npc");
	regPSR = sregister->getRegister("psr");
    regFSR = sregister->getRegister("fsr");
//    sregister->sparcRegisters->psr = castUnsignedLongToPSR(regPSR);
//    sregister->sparcRegisters->fsr = castUnsignedLongToPSR(regFSR);
    
    //struct processor_status_register psr = castUnsignedLongToPSR(regPSR);
    //struct floating_point_state_register fsr = castUnsignedLongToFSR(regFSR);
    
    // Strip off %hi to differentiate it from SETHI instruction
	for(index = 0; index < strlen(disassembledInstruction) - 3; index++)
	{
		if(disassembledInstruction[index] == '%' && disassembledInstruction[index + 1] == 'h' && disassembledInstruction[index + 2] == 'i')
		{
			disassembledInstruction[index] = ' ';
			disassembledInstruction[index + 1] = ' ';
			disassembledInstruction[index + 2] = ' ';
		}
	}

	strcpy(tokens[0], strtok(disassembledInstruction, " ,+[]()"));
	do
	{
		token = strtok(NULL, " ,[]%()");
		if(token != NULL)
			strcpy(tokens[++count], token);
	}while(token);
     
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
	// Format - I instruction
	if(!strcmp(tokens[0], "call"))
	{
		unsigned long displacement = strtoul(tokens[1], NULL, 0);
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc", displacement);
		sregister->setRegister("%o7", regPC);
		return RET_SUCCESS;
	}
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
	// Format - II instruction
	if(!strcmp(tokens[0], "sethi"))
	{
		sregister->setRegister(tokens[2], strtoul(tokens[1], NULL, 0));
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc", regnPC + 4);
		return RET_SUCCESS;
	}
	
	
	if(!strcmp(tokens[0], "nop"))
	{
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc", regnPC + 4);
		return RET_SUCCESS;
	}
	
	
	if(!strcmp(tokens[0], "unimp"))
	{
		printf("Attempt to execute UNIMP instruction");
		return RET_FAILURE;
	}
	

	if(!strcmp(tokens[0], "ba"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			regPC = strtoul(tokens[2], NULL, 0);
			sregister->setRegister("pc", regPC);
			sregister->setRegister("npc", regPC + 4);
		}
		else
		{
			// Annul bit = 0
			sregister->setRegister("pc", regnPC);
			regnPC = strtoul(tokens[1], NULL, 0);
			sregister->setRegister("npc", regnPC);
		}
		return RET_SUCCESS;
	}


	if(!strcmp(tokens[0], "bn"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			sregister->setRegister("pc", regnPC + 4);
			sregister->setRegister("npc", regnPC + 8);
		}
		else
		{
			// Annul bit = 0
			sregister->setRegister("pc", regnPC);
			sregister->setRegister("npc", regnPC + 4);
		}
		return RET_SUCCESS;
	}
		

	if(!(strcmp(tokens[0], "bne") &&
		strcmp(tokens[0], "be") &&
		strcmp(tokens[0], "bg") &&
		strcmp(tokens[0], "ble") &&
		strcmp(tokens[0], "bge") &&
		strcmp(tokens[0], "bl") &&
		strcmp(tokens[0], "bgu") &&
		strcmp(tokens[0], "bleu") &&
		strcmp(tokens[0], "bcc") &&
		strcmp(tokens[0], "bcs") &&
		strcmp(tokens[0], "bpos") &&
		strcmp(tokens[0], "bneg") &&
		strcmp(tokens[0], "bvc") &&
		strcmp(tokens[0], "bvs")))
	{
		unsigned short condition;

		if(!strcmp(tokens[0], "bne"))
			condition = !sregister->getPSR().getZ();
		else if(!strcmp(tokens[0], "be"))
			condition = sregister->getPSR().getZ();
		else if(!strcmp(tokens[0], "bg"))
			condition = !(sregister->getPSR().getZ() || (sregister->getPSR().getN() ^ sregister->getPSR().getV()));
		else if(!strcmp(tokens[0], "ble"))
			condition = sregister->getPSR().getZ() || (sregister->getPSR().getN() ^ sregister->getPSR().getV());
		else if(!strcmp(tokens[0], "bge"))
			condition = !(sregister->getPSR().getN() ^ sregister->getPSR().getV());
		else if(!strcmp(tokens[0], "bl"))
			condition = sregister->getPSR().getN() ^ sregister->getPSR().getV();
		else if(!strcmp(tokens[0], "bgu"))
			condition = !(sregister->getPSR().getC() || sregister->getPSR().getZ());
		else if(!strcmp(tokens[0], "bleu"))
			condition = sregister->getPSR().getC() || sregister->getPSR().getZ();
		else if(!strcmp(tokens[0], "bcc"))
			condition = !sregister->getPSR().getC();
		else if(!strcmp(tokens[0], "bcs"))
			condition = sregister->getPSR().getC();
		else if(!strcmp(tokens[0], "bpos"))
			condition = !sregister->getPSR().getN();
		else if(!strcmp(tokens[0], "bneg"))
			condition = sregister->getPSR().getN();
		else if(!strcmp(tokens[0], "bvc"))
			condition = !sregister->getPSR().getN();
		else if(!strcmp(tokens[0], "bvs"))
			condition = sregister->getPSR().getN();

		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			if(condition)
			{
				// Branch taken
				sregister->setRegister("pc", regnPC);
				regnPC = strtoul(tokens[2], NULL, 0);
				sregister->setRegister("npc", regnPC);
			}
			else
			{
				// Branch NOT taken
				sregister->setRegister("pc", regnPC + 4);
				sregister->setRegister("npc", regnPC + 8);
			}
		}
		else
		{
			// Annul bit = 0
			if(condition)
			{
				// Branch taken
				sregister->setRegister("pc", regnPC);
				regnPC = strtoul(tokens[1], NULL, 0);
				sregister->setRegister("npc", regnPC);
			}
			else
			{
				// Branch NOT taken
				sregister->setRegister("pc", regnPC);
				sregister->setRegister("npc", regnPC + 4);
			}
		}

		return RET_SUCCESS;
	}

        
    if(!strcmp(tokens[0], "fba"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			regPC = strtoul(tokens[2], NULL, 0);
			sregister->setRegister("pc", regPC);
			sregister->setRegister("npc", regPC + 4);
		}
		else
		{
			// Annul bit = 0
			sregister->setRegister("pc", regnPC);
			regnPC = strtoul(tokens[1], NULL, 0);
			sregister->setRegister("npc", regnPC);
		}
		return RET_SUCCESS;
	}


	if(!strcmp(tokens[0], "fbn"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			sregister->setRegister("pc", regnPC + 4);
			sregister->setRegister("npc", regnPC + 8);
		}
		else
		{
			// Annul bit = 0
			sregister->setRegister("pc", regnPC);
			sregister->setRegister("npc", regnPC + 4);
		}
		return RET_SUCCESS;
	}

        
    if(!(strcmp(tokens[0], "fbu") &&
		strcmp(tokens[0], "fbg") &&
		strcmp(tokens[0], "fbug") &&
		strcmp(tokens[0], "fbl") &&
		strcmp(tokens[0], "fbul") &&
		strcmp(tokens[0], "fblg") &&
		strcmp(tokens[0], "fbne") &&
		strcmp(tokens[0], "fbe") &&
		strcmp(tokens[0], "fbue") &&
		strcmp(tokens[0], "fbge") &&
		strcmp(tokens[0], "fbuge") &&
		strcmp(tokens[0], "fble") &&
		strcmp(tokens[0], "fbule") &&
		strcmp(tokens[0], "fbo")))
	{
		unsigned short condition;

		if(!strcmp(tokens[0], "fbu"))
			condition = (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fbg"))
			condition = (sregister->getFSR().getFcc() == FLOAT_GREATER);
		else if(!strcmp(tokens[0], "fbug"))
			condition = (sregister->getFSR().getFcc() == FLOAT_GREATER) || (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fbl"))
	    	condition = (sregister->getFSR().getFcc() == FLOAT_LESSER);
		else if(!strcmp(tokens[0], "fbul"))
			condition = (sregister->getFSR().getFcc() == FLOAT_LESSER) || (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fblg"))
			condition = (sregister->getFSR().getFcc() == FLOAT_LESSER) || (sregister->getFSR().getFcc() == FLOAT_GREATER);
		else if(!strcmp(tokens[0], "fbne"))
			condition = (sregister->getFSR().getFcc() == FLOAT_LESSER) || (sregister->getFSR().getFcc() == FLOAT_GREATER) || (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fbe"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL);
		else if(!strcmp(tokens[0], "fbue"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL) || (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fbge"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL) || (sregister->getFSR().getFcc() == FLOAT_GREATER);
		else if(!strcmp(tokens[0], "fbuge"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL) || (sregister->getFSR().getFcc() == FLOAT_GREATER) || (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fble"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL) || (sregister->getFSR().getFcc() == FLOAT_LESSER);
		else if(!strcmp(tokens[0], "fbule"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL) || (sregister->getFSR().getFcc() == FLOAT_LESSER) || (sregister->getFSR().getFcc() == FLOAT_UNORDERED);
		else if(!strcmp(tokens[0], "fbo"))
			condition = (sregister->getFSR().getFcc() == FLOAT_EQUAL) || (sregister->getFSR().getFcc() == FLOAT_LESSER) || (sregister->getFSR().getFcc() == FLOAT_GREATER);

		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			if(condition)
			{
				// Branch taken
				sregister->setRegister("pc", regnPC);
				regnPC = strtoul(tokens[2], NULL, 0);
				sregister->setRegister("npc", regnPC);
			}
			else
			{
				// Branch NOT taken
				sregister->setRegister("pc", regnPC + 4);
				sregister->setRegister("npc", regnPC + 8);
			}
		}
		else
		{
			// Annul bit = 0
			if(condition)
			{
				// Branch taken
				sregister->setRegister("pc", regnPC);
				regnPC = strtoul(tokens[1], NULL, 0);
				sregister->setRegister("npc", regnPC);
			}
			else
			{
				// Branch NOT taken
				sregister->setRegister("pc", regnPC);
				sregister->setRegister("npc", regnPC + 4);
			}
		}

		return RET_SUCCESS;
	}
        
        
	// Format - III instruction
        
  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

	// Decode Format - III operands for instruction having format: <opcode> [address], <regRD>
	index = 1;
	memoryAddress = getAddressValue(tokens, &index, sregister);
        
        
    if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldsb")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = memory->readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        if(other->getBit(word, 7))
            word = word | 0xFFFFFF00;
		sregister->setRegister(tokens[index], word);
		free(dataWord);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldsh")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

        if(other->is_mem_address_not_aligned(memoryAddress, HALFWORD_ALIGN))
        {
            printf("Source memory address not half word aligned");
            return RET_TRAP;
        }
                
		dataWord = memory->readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        if(other->getBit(word, 15))
            word = word | 0xFFFF0000;
		sregister->setRegister(tokens[index], word);
		free(dataWord);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldub")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = memory->readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		sregister->setRegister(tokens[index], word);
		free(dataWord);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "lduh")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

        if(other->is_mem_address_not_aligned(memoryAddress, HALFWORD_ALIGN))
        {
            printf("Source memory address not half word aligned");
            return RET_TRAP;
        }
                
		dataWord = memory->readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		sregister->setRegister(tokens[index], word);
		free(dataWord);
        }
	
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ld")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

        if(tokens[index][0] == 'f' && other->is_register_mis_aligned(tokens[index]))
        {
            // A floating-point load instruction
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        else
        if(other->is_register_mis_aligned(tokens[index]))
        {
            // An integer load instruction
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        if(other->is_mem_address_not_aligned(memoryAddress, WORD_ALIGN))
        {
            printf("Source memory address not word aligned");
            return RET_TRAP;
        }
                
		dataWord = memory->readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		sregister->setRegister(tokens[index], word);
		free(dataWord);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldd")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

        if(tokens[index][1] == 'f' && other->is_register_mis_aligned(tokens[index]))
        {
            // A floating-point load instruction
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        else if(other->is_register_mis_aligned(tokens[index]))
        {
            // An integer load instruction
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        if(other->is_mem_address_not_aligned(memoryAddress, DOUBLEWORD_ALIGN))
        {
            printf("Source memory address not double word aligned");
            return RET_TRAP;
        }
                
		dataWord = memory->readWordAsString(memoryAddress); 
        word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		sregister->setRegister(tokens[index], word);
        
        dataWord = memory->readWordAsString(memoryAddress + 4); 
        word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        sregister->setRegister(sregister->getNextRegister(tokens[index]), word);
		free(dataWord);
	}
        
   else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldstub")))
	{
        sregister->setRegister(tokens[index], memory->readByte(memoryAddress));
		memory->writeByte(memoryAddress, 0xFF);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "swap")))
	{
        unsigned long registerContent;
        
        if(other->is_mem_address_not_aligned(memoryAddress, WORD_ALIGN))
        {
            printf("Destination memory address not word aligned");
            return RET_TRAP;
        }
        
        registerContent = sregister->getRegister(tokens[index]);
        sregister->setRegister(tokens[index], memory->readWord(memoryAddress));
		memory->writeWord(memoryAddress, registerContent);
	}
	
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "jmpl")))
	{
		if(other->is_mem_address_not_aligned(memoryAddress, WORD_ALIGN))
        {
            printf("Destination memory address not word aligned");
            return RET_TRAP;
        }
        
        sregister->setRegister(tokens[index], regPC);
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc", memoryAddress);
		return RET_SUCCESS;
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "rett")))
	{
        if(sregister->getPSR().getEt() && !sregister->getPSR().getS())
        {
            printf("sregister->getPSR().getEt() = 1 and sregister->getPSR().getS() = 0");
            return RET_TRAP;
        }
        
        if(sregister->getPSR().getEt() && sregister->getPSR().getS())
        {
            printf("sregister->getPSR().getEt() = 1 and sregister->getPSR().getS() = 1");
            return RET_TRAP;
        }
        
        if(!sregister->getPSR().getEt())
        {
            if(!sregister->getPSR().getS())
            {
                printf("sregister->getPSR().getEt() = 0 and sregister->getPSR().getS() = 0");
                return RET_TRAP;
            }
            else
            if(other->is_mem_address_not_aligned(memoryAddress, WORD_ALIGN))
            {
                printf("Destination memory address not word aligned");
                return RET_TRAP;
            }
            else
            if(sregister->restoreRegisters() == RET_TRAP)
                return RET_TRAP;
        }
        
        sregister->setRegister(tokens[index], regPC);
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc", memoryAddress);
                
        sregister->getPSR().setS(sregister->getPSR().getPs());
        sregister->getPSR().setEt(1);
                
		return RET_SUCCESS;
	}
	
	if(!isFormatIIIOpcodeFound)
	{
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
	
	
	// Decode Format - III operands for instruction having format: <opcode> <regRD> , [address]
	index = 2;
	regRD = sregister->getRegister(tokens[1]);
	memoryAddress = getAddressValue(tokens, &index, sregister);
	
    if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "stb")))
	{
        char byte = regRD & 0x000000FF;
        memory->writeByte(memoryAddress, byte);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sth")))
	{
        unsigned short halfWord = regRD & 0x0000FFFF;
        
        if(other->is_mem_address_not_aligned(memoryAddress, HALFWORD_ALIGN))
        {
            printf("Destination memory address not half word aligned");
            return RET_TRAP;
        }
        
        memory->writeHalfWord(memoryAddress, halfWord);
	}
	
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "st")))
	{
        if(other->is_mem_address_not_aligned(memoryAddress, WORD_ALIGN))
        {
            printf("Destination memory address not word aligned");
            return RET_TRAP;
        }
        
        memory->writeWord(memoryAddress, regRD);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "std")))
	{
        unsigned long regNextRD;
        
        if(tokens[1][1] == 'f' && other->is_register_mis_aligned(tokens[1]))
        {
            // A floating-point load instruction
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        else
        if(other->is_register_mis_aligned(tokens[1]))
        {
            // An integer load instruction
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        if(other->is_mem_address_not_aligned(memoryAddress, DOUBLEWORD_ALIGN))
        {
            printf("Destination memory address not double word aligned");
            return RET_TRAP;
        }
        
        regNextRD = sregister->getRegister(sregister->getNextRegister(tokens[1]));
        memory->writeWord(memoryAddress, regRD);
        memory->writeWord(memoryAddress + 4, regNextRD);
	}
	
	
	if(!isFormatIIIOpcodeFound)
	{
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
	
        
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
	/*
         *  Decode Format - III operands for instruction having format: 
         *  1. <opcode> <regRS1>, <reg_or_imm>, <regRD> 
         *  2. <opcode> <regRS1>, <reg_or_imm>
         *  3. <opcode> <regRS1>, <regRS2>
         */
         
	regRS1 = sregister->getRegister(tokens[1]);
    regRS2 = sregister->getRegister(tokens[2]);
	reg_or_imm = getReg_Or_ImmValue(tokens[2], sregister);
        
    // Hex to single precision floating-point conversion
    convertFloat.floatToHex = regRS1;
    float_regRS1 = convertFloat.hexToFloat;
    convertFloat.floatToHex = regRS2;
    float_regRS2 = convertFloat.hexToFloat;
    
    // Hex to double precision floating-point conversion
    convertDouble.doubleToHex[0] = regRS1;
    convertDouble.doubleToHex[1] = sregister->getRegister(sregister->getNextRegister(tokens[1]));
    double_regRS1 = convertDouble.hexToDouble;
    convertDouble.doubleToHex[0] = regRS2;
    convertDouble.doubleToHex[1] = sregister->getRegister(sregister->getNextRegister(tokens[2]));
    double_regRS2 = convertDouble.hexToDouble;

	
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "add")))
        sregister->setRegister(tokens[3], regRS1 + reg_or_imm);

    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "addcc")))
	{
		regRD = regRS1 + reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
		updateICCAdd(regRS1, reg_or_imm, regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "addx")))
	{
		regRD = regRS1 + reg_or_imm + sregister->getPSR().getC();
		sregister->setRegister(tokens[3], regRD); 
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "addxcc")))
	{
		regRD = regRS1 + reg_or_imm + sregister->getPSR().getC();
		sregister->setRegister(tokens[3], regRD);
		updateICCAdd(regRS1, reg_or_imm, regRD, sregister);
	}
        
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sub")))
		sregister->setRegister(tokens[3], regRS1 - reg_or_imm);

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subcc")))
	{
		regRD = regRS1 - reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
		this->updateICCSubtract(regRS1, reg_or_imm, regRD, sregister);
	}

    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subx")))
	{
		regRD = regRS1 - reg_or_imm - sregister->getPSR().getC();
		sregister->setRegister(tokens[3], regRD); 
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subxcc")))
	{
		regRD = regRS1 - reg_or_imm - sregister->getPSR().getC();
		sregister->setRegister(tokens[3], regRD);
		this->updateICCSubtract(regRS1, reg_or_imm, regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "taddcc")))
	{
		regRD = regRS1 + reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
                taggedAddSubtract(regRS1, reg_or_imm, regRD, 0, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "taddcctv")))
	{
		regRD = regRS1 + reg_or_imm;
        if(taggedAddSubtract(regRS1, reg_or_imm, regRD, 1, sregister))
        {
            printf("Tag overflow has occurred");
            return RET_TRAP;
        }
        sregister->setRegister(tokens[3], regRD);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "tsubcc")))
	{
		regRD = regRS1 - reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
        taggedAddSubtract(regRS1, reg_or_imm, regRD, 0, sregister);
	}
        
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "tsubcctv")))
	{
		regRD = regRS1 - reg_or_imm;
        if(taggedAddSubtract(regRS1, reg_or_imm, regRD, 1, sregister))
        {
            printf("Tag overflow has occurred");
            return RET_TRAP;
        }
        sregister->setRegister(tokens[3], regRD);
	}
        
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "umul")))
	{
		unsigned long long extended_regRD;
		unsigned long regY;

		regY = 0, regRD = 0;
		extended_regRD = (unsigned long long)regRS1 * (unsigned long long)reg_or_imm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		sregister->setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		sregister->setRegister(tokens[3], regRD);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "smul")))
	{
		signed long long extended_regRD;
		unsigned long regY;
        signed long signedRegRS1 = (signed long)regRS1;
        signed long signedRegOrImm = (signed long)reg_or_imm;

		regY = 0, regRD = 0;
		extended_regRD = (signed long long)signedRegRS1 * (signed long long)signedRegOrImm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		sregister->setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		sregister->setRegister(tokens[3], regRD);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "umulcc")))
	{
		unsigned long long extended_regRD;
		unsigned long regY;

		regY = 0, regRD = 0;
		extended_regRD = (unsigned long long)regRS1 * (unsigned long long)reg_or_imm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		sregister->setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		sregister->setRegister(tokens[3], regRD);
        updateICCMulLogical(regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "smulcc")))
	{
		signed long long extended_regRD;
		unsigned long regY;
        signed long signed_regRS1 = (signed long)regRS1;
        signed long signed_reg_or_imm = (signed long)reg_or_imm;

		regY = 0, regRD = 0;
		extended_regRD = (signed long long)signed_regRS1 * (signed long long)signed_reg_or_imm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		sregister->setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		sregister->setRegister(tokens[3], regRD);
        updateICCMulLogical(regRD, sregister);
	}

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "udiv")))
	{
		unsigned long long dividend, quotient;
		unsigned long regY;

        if(reg_or_imm == 0)
        {
            printf("Attempt to divide by zero");
            return RET_TRAP;
        }
        
		regY = sregister->getRegister("y");
		// dividend = (dividend << 32) | regY;
        dividend = regY;
		dividend = (dividend << 32) | regRS1;
		quotient = dividend / reg_or_imm;

		if(quotient > ULONG_MAX)
			sregister->setRegister(tokens[3], 0xFFFFFFFF);
		else
			sregister->setRegister(tokens[3], (unsigned long)quotient);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sdiv")))
	{
		signed long long dividend, quotient;
        signed long long signed_reg_or_imm = (signed long long)reg_or_imm;
		unsigned long regY;

        if(reg_or_imm == 0)
        {
            printf("Attempt to divide by zero");
            return RET_TRAP;
        }
                
		regY = sregister->getRegister("y");
                dividend = regY;
		dividend = (dividend << 32) | regRS1; 
		quotient = dividend / signed_reg_or_imm;

		if(quotient > (signed long)0x7FFFFFFF)
			sregister->setRegister(tokens[3], 0x7FFFFFFF);    // Positive overflow
        else if(quotient < (signed long)0x80000000)
            sregister->setRegister(tokens[3], 0x80000000);    // Negative underflow
		else
			sregister->setRegister(tokens[3], (unsigned long)(quotient & 0x00000000FFFFFFFF));
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "udivcc")))
	{
		unsigned long long dividend, quotient;
		unsigned long regY;

        if(reg_or_imm == 0)
        {
            printf("Attempt to divide by zero");
            return RET_TRAP;
        }
        
		regY = sregister->getRegister("y");
		// dividend = (dividend << 32) | regY;
        dividend = regY;
		dividend = (dividend << 32) | regRS1;
		quotient = dividend / reg_or_imm;

		if(quotient > ULONG_MAX)
        {
			sregister->setRegister(tokens[3], 0xFFFFFFFF);
            updateICCDiv(0xFFFFFFFF, 1, sregister);
        }
		else
        {
			sregister->setRegister(tokens[3], (unsigned long)quotient);
            updateICCDiv((unsigned long)quotient, 0, sregister);
        }
	}

    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sdivcc")))
	{
		signed long long dividend, quotient;
        signed long long signed_reg_or_imm = (signed long long)reg_or_imm;
		unsigned long regY;

        if(reg_or_imm == 0)
        {
            printf("Attempt to divide by zero");
            return RET_TRAP;
        }
                
		regY = sregister->getRegister("y");
        dividend = regY;
		dividend = (dividend << 32) | regRS1; 
		quotient = dividend / signed_reg_or_imm;

		if(quotient > (signed long)0x7FFFFFFF)
        {
			sregister->setRegister(tokens[3], 0x7FFFFFFF);    // Positive overflow
            updateICCDiv(0x7FFFFFFF, 1, sregister);
        }
        else if(quotient < (signed long)0x80000000)
        {
            sregister->setRegister(tokens[3], 0x80000000);    // Negative underflow
            updateICCDiv(0x80000000, 1, sregister);
        }
		else
        {
			sregister->setRegister(tokens[3], (unsigned long)(quotient & 0x00000000FFFFFFFF));
            updateICCDiv((unsigned long)(quotient & 0x00000000FFFFFFFF), 0, sregister);
        }
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fadds")))
	{
        float_regRD = float_regRS1 + float_regRS2;
        convertFloat.hexToFloat = float_regRD;
		sregister->setRegister(tokens[3], convertFloat.floatToHex); 
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "faddd")))
	{
        if(other->is_register_mis_aligned(tokens[1]) || other->is_register_mis_aligned(tokens[2]) || other->is_register_mis_aligned(tokens[3]))
        {
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        double_regRD = double_regRS1 + double_regRS2;
        convertDouble.hexToDouble = double_regRD;
		sregister->setRegister(tokens[3], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsubs")))
	{
        float_regRD = float_regRS1 - float_regRS2;
        convertFloat.hexToFloat = float_regRD;
		sregister->setRegister(tokens[3], convertFloat.floatToHex); 
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsubd")))
	{
        if(other->is_register_mis_aligned(tokens[1]) || other->is_register_mis_aligned(tokens[2]) || other->is_register_mis_aligned(tokens[3]))
        {
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        double_regRD = double_regRS1 - double_regRS2;
        convertDouble.hexToDouble = double_regRD;
		sregister->setRegister(tokens[3], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fmuls")))
	{
        float_regRD = float_regRS1 * float_regRS2;
        convertFloat.hexToFloat = float_regRD;
		sregister->setRegister(tokens[3], convertFloat.floatToHex); 
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fmuld")))
	{
        if(other->is_register_mis_aligned(tokens[1]) || other->is_register_mis_aligned(tokens[2]) || other->is_register_mis_aligned(tokens[3]))
        {
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        double_regRD = double_regRS1 * double_regRS2;
        convertDouble.hexToDouble = double_regRD;
		sregister->setRegister(tokens[3], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdivs")))
	{
        float_regRD = float_regRS1 / float_regRS2;
        convertFloat.hexToFloat = float_regRD;
		sregister->setRegister(tokens[3], convertFloat.floatToHex);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdivd")))
	{
        if(other->is_register_mis_aligned(tokens[1]) || other->is_register_mis_aligned(tokens[2]) || other->is_register_mis_aligned(tokens[3]))
        {
            printf("Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        double_regRD = double_regRS1 / double_regRS2;
        convertDouble.hexToDouble = double_regRD;
		sregister->setRegister(tokens[3], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fmovs")))
        sregister->setRegister(tokens[2], regRS1);
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fnegs")))
        sregister->setRegister(tokens[2], other->toggleBit(regRS1, SIGN_BIT));
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fabss")))
        sregister->setRegister(tokens[2], other->clearBit(regRS1, SIGN_BIT));
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmps")))
	{
        if(other->isSNaN32(float_regRS1) || other->isSNaN32(float_regRS2))
        {
            printf("Either or both the operands are SNaN");
            return RET_TRAP;
        }
        if(float_regRS1 == float_regRS2)
            updateFCC(0, sregister);
        else
        if(float_regRS1 < float_regRS2)
            updateFCC(1, sregister);
        else
        if(float_regRS1 > float_regRS2)
            updateFCC(2, sregister);
        else
            updateFCC(3, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmpes")))
	{
        if(other->isSNaN32(float_regRS1) || other->isQNaN32(float_regRS1) || other->isSNaN32(float_regRS2) || other->isQNaN32(float_regRS2))
        {
            printf("Either or both the operands are SNaN/QNaN");
            return RET_TRAP;
        }
        if(float_regRS1 == float_regRS2)
            updateFCC(0, sregister);
        else
        if(float_regRS1 < float_regRS2)
            updateFCC(1, sregister);
        else
        if(float_regRS1 > float_regRS2)
            updateFCC(2, sregister);
        else
            updateFCC(3, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmpd")))
	{
        if(other->isSNaN64(double_regRS1) || other->isSNaN64(double_regRS1))
        {
            printf("Either or both the operands are SNaN");
            return RET_TRAP;
        }
        if(double_regRS1 == double_regRS2)
            updateFCC(0, sregister);
        else
        if(double_regRS1 < double_regRS2)
            updateFCC(1, sregister);
        else
        if(double_regRS1 > double_regRS2)
            updateFCC(2, sregister);
        else
            updateFCC(3, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmped")))
	{
        if(other->isSNaN64(double_regRS1) || other->isQNaN64(double_regRS1) || other->isSNaN64(double_regRS2) || other->isQNaN64(double_regRS2))
        {
            printf("Either or both the operands are SNaN/QNaN");
            return RET_TRAP;
        }
        if(double_regRS1 == double_regRS2)
            updateFCC(0, sregister);
        else
        if(double_regRS1 < double_regRS2)
            updateFCC(1, sregister);
        else
        if(double_regRS1 > double_regRS2)
            updateFCC(2, sregister);
        else
            updateFCC(3, sregister);
	}
        
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "and")))
		sregister->setRegister(tokens[3], regRS1 & reg_or_imm);

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andcc")))
	{
		regRD = regRS1 & reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
		updateICCMulLogical(regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andncc")))
	{
		regRD = regRS1 & (~reg_or_imm);
		sregister->setRegister(tokens[3], regRD);
		updateICCMulLogical(regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andn")))
		sregister->setRegister(tokens[3], regRS1 & (~reg_or_imm));

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "or")))
		sregister->setRegister(tokens[3], regRS1 | reg_or_imm);
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "orcc")))
	{
		regRD = regRS1 | reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
		updateICCMulLogical(regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "orn")))
		sregister->setRegister(tokens[3], regRS1 | (~reg_or_imm));

    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "orncc")))
	{
		regRD = regRS1 | (~reg_or_imm);
		sregister->setRegister(tokens[3], regRD);
		updateICCMulLogical(regRD, sregister);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xor")))
		sregister->setRegister(tokens[3], regRS1 ^ reg_or_imm);
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xorcc")))
	{
		regRD = regRS1 ^ reg_or_imm;
		sregister->setRegister(tokens[3], regRD);
		updateICCMulLogical(regRD, sregister);
	}
        
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xnor")))
		sregister->setRegister(tokens[3], ~(regRS1 ^ reg_or_imm));
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xnorcc")))
	{
		regRD = ~(regRS1 ^ reg_or_imm);
		sregister->setRegister(tokens[3], regRD);
		updateICCMulLogical(regRD, sregister);
	}

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sll")))
		sregister->setRegister(tokens[3], regRS1 << reg_or_imm);

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "srl")))
		sregister->setRegister(tokens[3], regRS1 >> reg_or_imm);
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sra")))
            sregister->setRegister(tokens[3], (regRS1 >> reg_or_imm) | (other->getBit(regRS1, 31) ? (0xFFFFFFFF << (32 - reg_or_imm)) : 0x00000000));

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "save")))
	{
		if(sregister->saveRegisters() == RET_TRAP)
			return RET_TRAP;
		else
			sregister->setRegister(tokens[3], regRS1 + reg_or_imm);
	}

	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "restore")))
	{
		if(sregister->restoreRegisters() == RET_TRAP)
			return RET_TRAP;
		else
			sregister->setRegister(tokens[3], regRS1 + reg_or_imm);
	}
	
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "rd")))
    {
	    if(!strcmp(tokens[1], "psr") && !sregister->getPSR().getS())
	    {
	        printf("Attempt to read PSR from user mode");
	        return RET_TRAP;
	    }
	    
	    if(!strcmp(tokens[1], "wim") && !sregister->getPSR().getS())
	    {
	        printf("Attempt to read WIM from user mode");
	        return RET_TRAP;
	    }
	    
	    if(!strcmp(tokens[1], "tbr") && !sregister->getPSR().getS())
	    {
	        printf("Attempt to read TBR from user mode");
	        return RET_TRAP;
	    }
	    
	    sregister->setRegister(tokens[2], sregister->getRegister(tokens[1]));
    }
        
	else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "wr")))
    {
		if(sregister->setRegister(tokens[3], regRS1 ^ reg_or_imm) == RET_TRAP)
        	return RET_TRAP;
    }
        
    if(!isFormatIIIOpcodeFound)
	{
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
               
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // Decode Format - III operands for instruction having format: <opcode> <regRS2>, <regRD>
    regRS2 = sregister->getRegister(tokens[1]);
    
    // Hex to single precision floating-point conversion
    convertFloat.floatToHex = regRS2;
    float_regRS2 = convertFloat.hexToFloat;
    
    // Hex to double precision floating-point conversion
    convertDouble.doubleToHex[0] = regRS2;
    convertDouble.doubleToHex[1] = sregister->getRegister(sregister->getNextRegister(tokens[1]));
    double_regRS2 = convertDouble.hexToDouble;
        
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fstoi")))
	{
        unsigned long roundedInteger;
        
        if(float_regRS2 > 0)
            roundedInteger = (unsigned long)floorf(float_regRS2);
        else
            roundedInteger = (unsigned long)ceilf(float_regRS2);
        
        sregister->setRegister(tokens[2], roundedInteger);
	}

    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdtoi")))
	{
        unsigned long long roundedInteger;
        
        if(double_regRS2 > 0)
            roundedInteger = (unsigned long)floor(double_regRS2);
        else
            roundedInteger = (unsigned long)ceil(double_regRS2);
       
        sregister->setRegister(tokens[2], (unsigned long)(roundedInteger & 0x00000000FFFFFFFF));
        sregister->setRegister(sregister->getNextRegister(tokens[2]), (unsigned long)(roundedInteger >> 32));
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fstod")))
	{
        convertDouble.hexToDouble = (double)float_regRS2;
        
        sregister->setRegister(tokens[2], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[2]), convertDouble.doubleToHex[1]);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdtos")))
	{
        convertFloat.hexToFloat = (float)double_regRS2;
       
        sregister->setRegister(tokens[2], convertFloat.floatToHex);
	}
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fitos")))
    {
        signed long signed_regRS2;
        
        signed_regRS2 = (signed long)regRS2;
        convertFloat.hexToFloat = (float)signed_regRS2;
       
        sregister->setRegister(tokens[2], convertFloat.floatToHex);
    }
        
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fitod")))
    {
        if(other->is_register_mis_aligned(tokens[1]) || other->is_register_mis_aligned(tokens[2]))
        {
            printf("Source/Destination is an odd-even register pair");
            return RET_TRAP;
        }
        
        signed long signed_regRS2;
        signed_regRS2 = (signed long)regRS2;
        convertDouble.hexToDouble = (double)signed_regRS2;
        sregister->setRegister(tokens[2], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[2]), convertDouble.doubleToHex[1]);
    }
    
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsqrts")))
    {
        convertFloat.hexToFloat = powf(float_regRS2, 0.5);
        sregister->setRegister(tokens[2], convertFloat.floatToHex);
    }
    
    else if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsqrtd")))
    {
        convertDouble.hexToDouble = pow(double_regRS2, 0.5);
        sregister->setRegister(tokens[2], convertDouble.doubleToHex[0]);
        sregister->setRegister(sregister->getNextRegister(tokens[2]), convertDouble.doubleToHex[1]);
    }
        
	if(!isFormatIIIOpcodeFound)
	{
		sregister->setRegister("pc", regnPC);
		sregister->setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}

	printf("Unimplemented in simulator: %s\n", tokens[0]);
	return RET_FAILURE;
}



/*
 * Returns the value contained in a register or sign-extended simm13 field
 * in instructions of the form "<opcode> <regRS1>, <reg_or_imm>, <regRS2>"
 */
unsigned long Execute::getReg_Or_ImmValue(char* reg_or_imm, Register *sregister)
{
        // The operand is a register.
	if((reg_or_imm[0] == '%') || (reg_or_imm[0] == 'g') || (reg_or_imm[0] == 'o') || (reg_or_imm[0] == 'l') || (reg_or_imm[0] == 'i'))
		return sregister->getRegister(reg_or_imm); 
        // The operand is an immediate.
	else
		// Note: strtoul() has been used here to interpret the bit pattern
		// as signed long instead of unsigned long as translated by strtol()
		return strtoul(reg_or_imm, NULL, 0);
}


/*
 * Returns the value specified as "[<register1> + <register2>]" or
 * "[<register> + <immediate>]".
 */
unsigned long Execute::getAddressValue(char tokens[][20], unsigned short* index, Register *sregister)
{
	unsigned long memoryAddress;

        // Search for opening bracket ([)
	if(!strcmp(tokens[*index], "["))
		(*index)++;
	
        // get the value in first register
	memoryAddress = sregister->getRegister(tokens[*index]);

        // Search for addition symbol (+) to get the value of second register or immediate.
	if(!strcmp(tokens[++(*index)], "+"))
		memoryAddress += getReg_Or_ImmValue(tokens[++(*index)], sregister);

        // Search for closing bracket (])
	(*index) += (!strcmp(tokens[(*index) + 1], "]")) ? 2 : 1;

	return memoryAddress;
}



/*
 * Updates Integer Condition Code (ICC) bits based on the result of addition.
 */
void Execute::updateICCAdd(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, Register *sregister)
{
	/* Disambiguation between CARRY and OVERFLOW flag:
	   1. http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	   2. http://www.c-jump.com/CIS77/CPU/Overflow/lecture.html 
        */

	unsigned long regPSR;
	unsigned short signBit_regRS1, signBit_reg_or_imm, signBit_regRD;
	

	regPSR = sregister->getRegister("psr");
	signBit_regRS1 = other->getBit(regRS1, SIGN_BIT);
	signBit_reg_or_imm = other->getBit(reg_or_imm, SIGN_BIT);
	signBit_regRD = other->getBit(regRD, SIGN_BIT);

	// Set ICC_NEGATIVE (n) bit
	regPSR = other->getBit(regRD, SIGN_BIT) ? other->setBit(regPSR, ICC_NEGATIVE) : other->clearBit(regPSR, ICC_NEGATIVE);
	

	// Set ICC_ZERO (z) bit
	regPSR = (regRD == 0) ? other->setBit(regPSR, ICC_ZERO) : other->clearBit(regPSR, ICC_ZERO);


	// Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
        regPSR = ((signBit_regRS1 && signBit_reg_or_imm && !signBit_regRD) || (!signBit_regRS1 && !signBit_reg_or_imm && signBit_regRD)) ? other->setBit(regPSR, ICC_OVERFLOW) : other->clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
        regPSR = (signBit_regRS1 && signBit_reg_or_imm) || (!signBit_regRD && (signBit_regRS1 || signBit_reg_or_imm)) ? other->setBit(regPSR, ICC_CARRY) : other->clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	sregister->setRegister("psr", regPSR);
}



/*
 * Updates Integer Condition Code (ICC) bits based on the result of subtraction.
 */
void Execute::updateICCSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, Register *sregister)
{
	/* Disambiguation between CARRY and OVERFLOW flag:
	   1. http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	   2. http://www.c-jump.com/CIS77/CPU/Overflow/lecture.html 
        */

	unsigned long regPSR;
	unsigned short signBit_regRS1, signBit_reg_or_imm, signBit_regRD;
	

	regPSR = sregister->getRegister("psr");
	signBit_regRS1 = other->getBit(regRS1, SIGN_BIT);
	signBit_reg_or_imm = other->getBit(reg_or_imm, SIGN_BIT);
	signBit_regRD = other->getBit(regRD, SIGN_BIT);

	// Set ICC_NEGATIVE (n) bit
	regPSR = other->getBit(regRD, SIGN_BIT) ? other->setBit(regPSR, ICC_NEGATIVE) : other->clearBit(regPSR, ICC_NEGATIVE);
	

	// Set ICC_ZERO (z) bit
	regPSR = (regRD == 0) ? other->setBit(regPSR, ICC_ZERO) : other->clearBit(regPSR, ICC_ZERO);


	// Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
	regPSR = ((signBit_regRS1 && (!signBit_reg_or_imm && !signBit_regRD)) || (!signBit_regRS1 && (signBit_reg_or_imm && signBit_regRD))) ? other->setBit(regPSR, ICC_OVERFLOW) : other->clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
        regPSR = (!signBit_regRS1 && signBit_reg_or_imm) || (signBit_regRD && (!signBit_regRS1 || signBit_reg_or_imm)) ? other->setBit(regPSR, ICC_CARRY) : other->clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	sregister->setRegister("psr", regPSR);
}


/*
 * Updates Integer Condition Code (ICC) bits based on the result of tagged addition and subtraction.
 * If isTVOpcode = 1, the opcodes will be treated as TADDCCTV and TSUBCCTV.
 * If isTVOpcode = 0, the opcodes will be treated as TADDCC and TSUBCC.
 * Returns 1, if tagged overflow has occurred.
 */
unsigned short Execute::taggedAddSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, unsigned short isTVOpcode, Register *sregister)
{
    unsigned long regPSR;
	unsigned short signBit_regRS1, signBit_reg_or_imm, signBit_regRD, isOperandsLSBNonZero, isTaggedOverflow;
	

	regPSR = sregister->getRegister("psr");
	signBit_regRS1 = other->getBit(regRS1, SIGN_BIT);
	signBit_reg_or_imm = other->getBit(reg_or_imm, SIGN_BIT);
	signBit_regRD = other->getBit(regRD, SIGN_BIT);
    isOperandsLSBNonZero = other->getBit(regRS1, 0) | other->getBit(regRS1, 1) | other->getBit(reg_or_imm, 0) | other->getBit(reg_or_imm, 1); 
    
    // Set ICC_OVERFLOW (v) bit: Important for TAGGED arithmetic
    isTaggedOverflow = ((signBit_regRS1 && signBit_reg_or_imm && !signBit_regRD) || (!signBit_regRS1 && !signBit_reg_or_imm && signBit_regRD) || isOperandsLSBNonZero) ? 1 : 0;
        
    if(isTaggedOverflow && isTVOpcode)
        return 1;
    else
    {
        // Set ICC_NEGATIVE (n) bit
        regPSR = other->getBit(regRD, SIGN_BIT) ? other->setBit(regPSR, ICC_NEGATIVE) : other->clearBit(regPSR, ICC_NEGATIVE);


        // Set ICC_ZERO (z) bit
        regPSR = (regRD == 0) ? other->setBit(regPSR, ICC_ZERO) : other->clearBit(regPSR, ICC_ZERO);


        // Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
        regPSR = isTaggedOverflow ? other->setBit(regPSR, ICC_OVERFLOW) : other->clearBit(regPSR, ICC_OVERFLOW);
        

        // Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
        regPSR = (!signBit_regRS1 && signBit_reg_or_imm) || (signBit_regRD && (!signBit_regRS1 || signBit_reg_or_imm)) ? other->setBit(regPSR, ICC_CARRY) : other->clearBit(regPSR, ICC_CARRY);


        // Set PSR back to modify ICC bits
        sregister->setRegister("psr", regPSR);
    }
    
    return isTaggedOverflow;
                
}



/*
 * Updates Integer Condition Code (ICC) bits based on the result of multiplication and logical operations.
 */
void Execute::updateICCMulLogical(unsigned long regRD, Register *sregister)
{
	unsigned long regPSR;	

	regPSR = sregister->getRegister("psr");

	// Set ICC_NEGATIVE (n) bit
	regPSR = other->getBit(regRD, SIGN_BIT) ? other->setBit(regPSR, ICC_NEGATIVE) : other->clearBit(regPSR, ICC_NEGATIVE);
	

	// Set ICC_ZERO (z) bit
	regPSR = (regRD == 0) ? other->setBit(regPSR, ICC_ZERO) : other->clearBit(regPSR, ICC_ZERO);


	// Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
	regPSR = other->clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
	regPSR = other->clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	sregister->setRegister("psr", regPSR);
}



/*
 * Updates Integer Condition Code (ICC) bits based on the result of division.
 */
void Execute::updateICCDiv(unsigned long regRD, short isOverflow, Register *sregister)
{
	unsigned long regPSR;

	regPSR = sregister->getRegister("psr");

	// Set ICC_NEGATIVE (n) bit
	regPSR = other->getBit(regRD, SIGN_BIT) ? other->setBit(regPSR, ICC_NEGATIVE) : other->clearBit(regPSR, ICC_NEGATIVE);
	

	// Set ICC_ZERO (z) bit
	regPSR = (regRD == 0) ? other->setBit(regPSR, ICC_ZERO) : other->clearBit(regPSR, ICC_ZERO);


	// Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
	regPSR = isOverflow ? other->setBit(regPSR, ICC_OVERFLOW) : other->clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
	regPSR = other->clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	sregister->setRegister("psr", regPSR);
}



/*
 * Updates Floating-point Condition Code (FCC) bits.
 */
void Execute::updateFCC(unsigned short fcc, Register *sregister)
{
	unsigned long regFSR;
	
        regFSR = sregister->getRegister("fsr");
        switch(fcc)
        {
            case 0: regFSR = other->clearBit(regFSR, 11); regFSR = other->clearBit(regFSR, 10); break;
            case 1: regFSR = other->clearBit(regFSR, 11); regFSR = other->setBit(regFSR, 10); break;
            case 2: regFSR = other->setBit(regFSR, 11); regFSR = other->clearBit(regFSR, 10); break;
            case 3: regFSR = other->setBit(regFSR, 11); regFSR = other->setBit(regFSR, 10); break;
        }

	// Set FSR back to modify FCC bits
	sregister->setRegister("fsr", regFSR);
}



/*
 * Serves as a wrapper around executeInstruction() method.
 * Each time it is invoked, it checks whether the PC value 
 * has hit a breakpoint, if not then disassembles the instruction 
 * word, saves information about the instruction to be executed,
 * executes it and returns a success or failure.
 */
 
/* 
int Execute::executeNextInstruction(Memory memory, Register sregister)
{
    char *cpuInstruction, *disassembledInstruction;
    unsigned long regPC;
    signed int exitCode;
    
    // get the PC value
    regPC = sregister->getRegister("pc");
    
    // Fetch the instruction word
    cpuInstruction = memory->readWordAsString(regPC);
    
    // Disassemble the instruction
    disassembledInstruction = (char*)decodeInstruction(cpuInstruction, regPC);
    
    // Save instruction info
    lastInstructionInfo.regPC = regPC;
    strcpy(lastInstructionInfo.cpuInstruction, cpuInstruction);
    strcpy(lastInstructionInfo.disassembledInstruction, disassembledInstruction);
    
    // Execute instruction
    exitCode = executeInstruction(disassembledInstruction, memory, sregister);
    
    // Free up grabbed memory, prevent memory leak
    free(cpuInstruction);
    free(disassembledInstruction);
    
    return exitCode;
} */
