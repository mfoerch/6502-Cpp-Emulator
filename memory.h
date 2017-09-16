#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>

using namespace std;

class Memory {
private:
    uint8_t ram[65536];

public:
    Memory();
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);

    void loadROMFile(string filename, uint16_t startaddress);

    void debugMemory(uint16_t start_address, uint16_t bytes);
};

#endif // MEMORY_H

