#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <libelf.h>
#include <gelf.h>
#include <fenv.h>
#include <limits.h>

using namespace std;

class Simulator{
    private:
        void resetSimulator();

};