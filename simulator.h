#include "header.h"
#include "reader.h"
#include "memory.h"
#include "decode.h"
#include "other.h"

class Simulator{
    private:
        char* elfBinary;
        Reader *reader;
        Memory *memory;
        Decode *decode;
    public:
        Simulator(char*);
        void startSimulation();

};


