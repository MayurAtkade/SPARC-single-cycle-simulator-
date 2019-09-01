#include "header.h"

struct loadedSections
{
	char sectionName[15];
	unsigned long sectionLoadAddress;
	short sectionType;
	unsigned long sectionSize;
	unsigned long instructionCount;
	struct loadedSections* nextSection;
};

class Reader{
    public:
        int initializeLoader(char *elfBinary);
        struct loadedSections* load_sparc_instructions(char *elfBinary);

};