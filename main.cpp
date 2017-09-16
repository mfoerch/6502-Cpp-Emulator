#include <iostream>
#include "memory.h"

#include "cpu.h"

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

    while(cpu->executeOpcode());

    //mem->debugMemory( 0x0400 , 256 );

    mem->debugMemory( 0x0000, 256);
    mem->debugMemory( 0x0100, 256);

    cout << hex << (int) mem->read(0x2000) << endl;


}
