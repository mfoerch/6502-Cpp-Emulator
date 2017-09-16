/********************************

    Developer note:
    The emulator can run a testprogramm which tests the complete documented(!) behaviour - Passed!

********************************/


#include <iostream>
#include "memory.h"

#include "cpu.h"

#include <time.h>

using namespace std;


int main()
{
    //Memory* mem = new Memory();

    Memory* mem = new Memory();
    mem->loadROMFile("6502_functional_test.bin", 0x0000);



    Cpu* cpu = new Cpu();

    cpu->attachMemory(mem);

    //mem->write(0x2005, 0xF0);




    cpu->reset(0x0400);

    unsigned long int cycleCounter = 0;

    time_t start, finish;

    cout << "Starting test" << endl;
    start = clock();

    while(cpu->executeOpcode())
    {
        cycleCounter += cpu->getLastCycles();
    }

    finish = clock();
    float timeDiff = (float) (finish-start) / CLOCKS_PER_SEC;

    double freq = cycleCounter / timeDiff;    //CLKs per second



    cout << "Took " << dec << cycleCounter << " Cycles to execute testfile!" << endl;
    cout << "Time: " << timeDiff << endl;
    cout << "Emulated frequency: " << freq << "Hz   " << freq / 1000000 << "MHz" << endl;



    mem->debugMemory( 0x0000, 256);
    mem->debugMemory( 0x0100, 256);

    cout << hex << (int) mem->read(0x2000) << endl;


}
