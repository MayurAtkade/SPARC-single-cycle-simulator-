#include "header.h"
#include "constants.h"
struct processor_status_register
{
	unsigned int cwp:5;
	unsigned int et:1;
	unsigned int ps:1;
	unsigned int s:1;
	unsigned int pil:4;
	unsigned int ef:1;
	unsigned int ec:1;
	unsigned int reserved:6;
	   
	unsigned int c:1;
	unsigned int v:1;
	unsigned int z:1;
	unsigned int n:1;

	unsigned int ver:4;
	unsigned int impl:4;
}__attribute__ ((__packed__));



struct floating_point_state_register
{
	unsigned int cexc:5;
	unsigned int aexc:5;
	unsigned int fcc:2;
	unsigned int ulow:1;
	unsigned int qne:1;
	unsigned int ftt:3;
	unsigned int ver:3;
	unsigned int res:2;
	unsigned int ns:1;
	unsigned int tem:5;
	unsigned int uhigh:2;
	unsigned int rd:2;
}__attribute__ ((__packed__));



struct registers
{
       unsigned long* registerSet;
       unsigned long* globalRegisters;
       unsigned long* cwptr;
       unsigned long wim, tbr, y, pc, npc;
       unsigned long asrRegisters[32];
       unsigned long floatingPointRegisters[32];
       unsigned short registerWindows;
       
       /* Though Intel x86 architecture allows un-aligned memory access, SPARC mandates memory accesses to be 8 byte aligned.
        Without __attribute__ ((aligned (8))) or a preceding dummy byte e.g. unsigned short dummyByte, the code below crashes 
        with a dreaded Bus error and Core dump. For more details, follow the links below:
        
        http://blog.jgc.org/2007/04/debugging-solaris-bus-error-caused-by.html
        https://groups.google.com/forum/?fromgroups=#!topic/comp.unix.solaris/8SgFiMudGL4 */
       
       struct processor_status_register __attribute__ ((aligned (8))) psr;
       struct floating_point_state_register __attribute__ ((aligned (8))) fsr;
}__attribute__ ((__packed__));

class Register{
    public:
        struct registers sparcRegisters;
        void initializeRegisters();
        char* displayRegister(unsigned long registerValue);
        unsigned short getRegisterWindow();
        void setRegisterWindow(unsigned short registerWindow);
        unsigned long* getWindowPointer(int direction);
        unsigned long getRegister(char* sparcRegister);
        int setRegister(char* sparcRegister, unsigned long registerValue);
        unsigned long getPSR();
        void setPSR(unsigned long psrValue);
        unsigned long getTBR();
        void setTBR(unsigned long tbrValue);
        int saveRegisters();
        int restoreRegisters();
        void nextWindow();
        unsigned long castPSRToUnsignedLong(struct processor_status_register psr);
        struct processor_status_register castUnsignedLongToPSR(unsigned long registerValue);
        struct floating_point_state_register castUnsignedLongToFSR(unsigned long registerValue);
        unsigned long castFSRToUnsignedLong(struct floating_point_state_register fsr);
        char* getNextRegister(char* sparcRegister);

};
