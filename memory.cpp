#include "memory.h"

#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>

using namespace std;

Memory::Memory()
{
    for (int i = 0; i < 65536; i++)
    {
        ram[i] = 0;
    }
}

uint8_t Memory::read(uint16_t address)
{
    return ram[address];
}

void Memory::write(uint16_t address, uint8_t value)
{
    ram[address] = value;
    if (address == 0x2b7a)
    {
        cout << "Error wrote to mem location 0x2005!!!!" << endl;
        usleep(1000000);
        exit(0);
    }
}

void Memory::loadROMFile(string filename, uint16_t startaddress)
{
    fstream file;
    file.open(filename.c_str(), ios::binary | ios::in);

    /*uint8_t adrLow;
    file >> adrLow;

    uint8_t adrHigh;
    file >> adrHigh;
    */

    //uint16_t i = adrLow | (adrHigh << 8);
    uint16_t i = 0;

    while (file.good())
    {
        ram[i] = file.get();
        i++;
    }

    cout << endl;

}

void Memory::debugMemory(uint16_t start_address, uint16_t bytes)
{
    cout << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "Debug Memory from $" << hex << (int) start_address << " to $" << (int) start_address + bytes - 1 << " *********** " << endl;
    cout << endl;

    uint16_t bytecounter = 0;

    uint8_t colCounter = 0;

    uint16_t currentAddress = start_address;

    do {
        if (colCounter == 0)
        {
            /* Output Row Address */
            cout << setfill('0') << setw(4) << (int) currentAddress << " : ";
        }

        /* Output value */
        cout << setfill('0') << setw(2) << (int) ram[currentAddress] << " ";

        currentAddress++;
        bytecounter++;

        colCounter++;

        if (colCounter == 8)
        {
            cout << " ";
        }
        if (colCounter == 16)
        {
            colCounter = 0;
            cout << endl;
        }
    } while (bytecounter < bytes);

}

