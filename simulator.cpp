#include "simulator.h"
#include "other.h"

Simulator::Simulator(char* f){
    elfBinary=f;
    r=new Reader();
    mem=new Memory();
    d=new Decode();
}

void Simulator::startSimulation(){
    struct loadedSections *elfSectionsPrevPtr, *elfSectionCurPtr;
    other *o=new other();
    elfSectionCurPtr = r->load_sparc_instructions(elfBinary);
    do
	{
		printf("Disassembly of Section: %s\n", elfSectionCurPtr->sectionName);
		printf("Section Load Address: %lu\n", elfSectionCurPtr->sectionLoadAddress);
		printf("Instruction Count: %lu\n",elfSectionCurPtr->instructionCount);
		if(elfSectionCurPtr->sectionType == 1)
            mem->set_lastMemory(elfSectionCurPtr->instructionCount*4);
		printf("Section Type: %u\n", elfSectionCurPtr->sectionType);
		printf("Section Size: %lu\n",elfSectionCurPtr->sectionSize);

		elfSectionsPrevPtr = elfSectionCurPtr;
		free(elfSectionsPrevPtr);
		elfSectionCurPtr = elfSectionCurPtr->nextSection;
	}
	
	while(elfSectionCurPtr != NULL);
	printf("\n");

	int firstNumericParametre = 0;
	int secondNumericParametre = mem->get_curMemory();

	/*// Initializing execution environment
	setRegister("pc", firstNumericParametre);
	setRegister("npc", firstNumericParametre + 4);
	setRegister("i6", 0x40400000);
	setRegister("o6", 0x403FFE80);
	setRegister("wim", 0x0000002);
	setRegister("psr", 0xF30010E0);*/
	
	unsigned long instructionCount;

	for(instructionCount = 0; instructionCount < mem->get_lastMemory; instructionCount++)
	{
		char* cpuInstruction = NULL;
		char* disassembledInstruction = NULL;
		char* hexNumber = (char*)malloc(32);
		cpuInstruction = mem->readWordAsString(firstNumericParametre);
		int wrong_instruction = 0;
		disassembledInstruction = (char*)d->decodeInstruction(cpuInstruction, firstNumericParametre);
        if(strcmp("-1",disassembledInstruction)==0) wrong_instruction=1;
		if (wrong_instruction == 0)	
		{	
			printf("\n\t");
			sprintf(hexNumber, "%lx", firstNumericParametre);
			printf("0x%s:\t", hexNumber);
			o->displayWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);
		}		
		firstNumericParametre += 4;
		free(cpuInstruction);
		free(disassembledInstruction);
		free(hexNumber);
	}
	printf("\n\n");

}