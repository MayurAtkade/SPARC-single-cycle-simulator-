#include "header.h"
#include "reader.h"
#include "memory.h"
#include "decode.h"
#include "other.h"
#include "register.h"
#include "execute.h"

class Simulator{
    private:
        char* elfBinary;
        Reader *reader;
        Memory *memory;
        Decode *decode;
        Register *sregister;
        Execute *execute;
    public:
        Simulator(char*);
        void startSimulation();

};


