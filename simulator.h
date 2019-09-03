#include "header.h"
#include "reader.h"
#include "memory.h"
#include "decode.h"

class Simulator{
    private:
        char* elfBinary;
        static Reader *reader;
        static Memory *memory;
        static Decode *decode;
    public:
        Simulator(char*);
        void startSimulation();

};


