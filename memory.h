#include "header.h"

class Memory{
    private:
        int *memory;
        int curMemory,lastMemory;
    public:
        Memory(){};
        void set_lastMemory(unsigned long);
        unsigned long get_lastMemory();
        void set_curMemory(unsigned long);
        unsigned long get_curMemory();
        char* readWordAsString(unsigned long memoryAddress);
};