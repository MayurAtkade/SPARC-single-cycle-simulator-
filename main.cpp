#include "simulator.h"

int main(int argc, char* argv[])
{
    
    Simulator *s=new Simulator(argv[1]);
    s->startSimulation();
    return 0;
}