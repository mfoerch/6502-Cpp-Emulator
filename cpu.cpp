#include "cpu.h"

#define     CPU_DEBUG_INSTRUCTION_TEXT

#include <iostream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

uint8_t lastOpcode = 0;

uint16_t debugPoint = 0xFFFF;//0x3500;//0x1c00;
const uint16_t breakpoint = 0x3399;//0x3509;//0x3509;//0x3509;//0x2b7a;//0x2a47;//0x2015;//0x1b21;//0x0ebd; //0xd78;//0x36fd;//0x36ea; //0x36e5;

bool debugEnabled = false;
bool infiniteBranch = false;


Cpu::Cpu()
{
    initializeTable();
}

void Cpu::attachMemory(Memory* mem)
{
    memory = mem;
}

void Cpu::reset(uint16_t PC_position)
{
    sp = 0;
    sreg = 0x30;
    a = 0;
    x = 0;
    y = 0;
    pc = PC_position;
    debugEnabled = false;
}


void Cpu::initializeTable()
{
    for (unsigned int i = 0; i < 256; i++)
    {
        debStr[i][0] = "";
        debStr[i][1] = "";
        cycleTime[i] = 0;
    }

#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debStr[0x00][0] = "BRK";
    debStr[0x01][0] = "ORA ($"; debStr[0x01][1] = ",X)";
    debStr[0x02][0] = "KIL";
    debStr[0x03][0] = "ILL_SLO izx";
    debStr[0x04][0] = "ILL_NOP";
    debStr[0x05][0] = "ORA $";
    debStr[0x06][0] = "ASL";
    debStr[0x07][0] = "SLO zp";
    debStr[0x08][0] = "PHP";
    debStr[0x09][0] = "ORA #$";
    debStr[0x0A][0] = "ASL A";
    debStr[0x0B][0] = "ILL_ANC imm";
    debStr[0x0C][0] = "ILL_NOP";
    debStr[0x0D][0] = "ORA $";
    debStr[0x0E][0] = "ASL";
    debStr[0x0F][0] = "ILL_SLO abs";

    debStr[0x10][0] = "BPL";
    debStr[0x11][0] = "ORA ($"; debStr[0x11][1] = "),Y";
    debStr[0x12][0] = "KIL";
    debStr[0x13][0] = "ILL_SLO izy";
    debStr[0x14][0] = "ILL_NOP";
    debStr[0x15][0] = "ORA $";  debStr[0x15][1] = ",X";
    debStr[0x16][0] = "ASL";
    debStr[0x17][0] = "ILL_SLO zpx";
    debStr[0x18][0] = "CLC";
    debStr[0x19][0] = "ORA $";  debStr[0x19][1] = ",Y";
    debStr[0x1A][0] = "ILL_NOP";
    debStr[0x1B][0] = "ILL_SLO aby";
    debStr[0x1C][0] = "ILL_NOP";
    debStr[0x1D][0] = "ORA $";  debStr[0x1D][1] = ",X";
    debStr[0x1E][0] = "ASL";
    debStr[0x1F][0] = "ILL_SLO abx";

    debStr[0x20][0] = "JSR $";
    debStr[0x21][0] = "AND ($"; debStr[0x21][1] = ",X)";
    debStr[0x22][0] = "KIL";
    debStr[0x23][0] = "ILL_RLA izx";
    debStr[0x24][0] = "BIT $";
    debStr[0x25][0] = "AND $";
    debStr[0x26][0] = "ROL";
    debStr[0x27][0] = "ILL_RLA zp";
    debStr[0x28][0] = "PLP";
    debStr[0x29][0] = "AND #$";
    debStr[0x2A][0] = "ROL A";
    debStr[0x2B][0] = "ILL_ANC imm";
    debStr[0x2C][0] = "BIT $";
    debStr[0x2D][0] = "AND $";
    debStr[0x2E][0] = "ROL";
    debStr[0x2F][0] = "ILL_RLA abs";

    debStr[0x30][0] = "BMI";
    debStr[0x31][0] = "AND ($"; debStr[0x31][1] = "),Y";
    debStr[0x32][0] = "KIL";
    debStr[0x33][0] = "ILL_RLA izy";
    debStr[0x34][0] = "ILL_NOP";
    debStr[0x35][0] = "AND $";  debStr[0x35][1] = ",X";
    debStr[0x36][0] = "ROL";
    debStr[0x37][0] = "ILL_RLA zpx";
    debStr[0x38][0] = "SEC";
    debStr[0x39][0] = "AND $";  debStr[0x39][1] = ",Y";
    debStr[0x3A][0] = "ILL_NOP";
    debStr[0x3B][0] = "ILL_RLA aby";
    debStr[0x3C][0] = "ILL_NOP";
    debStr[0x3D][0] = "AND $";  debStr[0x3D][1] = ",X";
    debStr[0x3E][0] = "ROL";
    debStr[0x3F][0] = "ILL_RLA abx";

    debStr[0x40][0] = "RTI";
    debStr[0x41][0] = "EOR ($"; debStr[0x41][1] = ",X)";
    debStr[0x42][0] = "KIL";
    debStr[0x43][0] = "ILL_SRE izx";
    debStr[0x44][0] = "ILL_NOP";
    debStr[0x45][0] = "EOR $";
    debStr[0x46][0] = "LSR";
    debStr[0x47][0] = "ILL_SRE zp";
    debStr[0x48][0] = "PHA";
    debStr[0x49][0] = "EOR #$";
    debStr[0x4A][0] = "LSR A";
    debStr[0x4B][0] = "ILL_ALR imm";
    debStr[0x4C][0] = "JMP $";
    debStr[0x4D][0] = "EOR $";
    debStr[0x4E][0] = "LSR";
    debStr[0x4F][0] = "ILL_SRE abs";

    debStr[0x50][0] = "BVC";
    debStr[0x51][0] = "EOR ($"; debStr[0x51][1] = "),Y";
    debStr[0x52][0] = "KIL";
    debStr[0x53][0] = "ILL_SRE izy";
    debStr[0x54][0] = "ILL_NOP zpx";
    debStr[0x55][0] = "EOR $";  debStr[0x55][1] = ",X";
    debStr[0x56][0] = "LSR";
    debStr[0x57][0] = "ILL_SRE zpx";
    debStr[0x58][0] = "CLI";
    debStr[0x59][0] = "EOR $";  debStr[0x59][1] = ",Y";
    debStr[0x5A][0] = "ILL_NOP";
    debStr[0x5B][0] = "ILL_SRE aby";
    debStr[0x5C][0] = "ILL_NOP";
    debStr[0x5D][0] = "EOR $";  debStr[0x5D][1] = ",X";
    debStr[0x5E][0] = "LSR $";  debStr[0x5E][1] = ",X";
    debStr[0x5F][0] = "ILL_SRE abx";

    debStr[0x60][0] = "RTS";
    debStr[0x61][0] = "ADC ($"; debStr[0x61][1] = ",X)";
    debStr[0x62][0] = "KIL";
    debStr[0x63][0] = "ILL_RRA izx";
    debStr[0x64][0] = "ILL_NOP";
    debStr[0x65][0] = "ADC $";
    debStr[0x66][0] = "ROR";
    debStr[0x67][0] = "ILL_RRA zp";
    debStr[0x68][0] = "PLA";
    debStr[0x69][0] = "ADC #$";
    debStr[0x6A][0] = "ROR A";
    debStr[0x6B][0] = "ILL_ARR imm";
    debStr[0x6C][0] = "JMP ($"; debStr[0x6C][1] = ")";
    debStr[0x6D][0] = "ADC $";
    debStr[0x6E][0] = "ROR";
    debStr[0x6F][0] = "ILL_RRA abs";

    debStr[0x70][0] = "BVS";
    debStr[0x71][0] = "ADC ($"; debStr[0x71][1] = "),Y";
    debStr[0x72][0] = "KIL";
    debStr[0x73][0] = "ILL_RRA izy";
    debStr[0x74][0] = "ILL_NOP";
    debStr[0x75][0] = "ADC $";  debStr[0x75][1] = ",X";
    debStr[0x76][0] = "ROR";
    debStr[0x77][0] = "ILL_RRA zpx";
    debStr[0x78][0] = "SEI";
    debStr[0x79][0] = "ADC $";  debStr[0x79][1] = ",Y";
    debStr[0x7A][0] = "ILL_NOP";
    debStr[0x7B][0] = "ILL_RRA aby";
    debStr[0x7C][0] = "ILL_NOP";
    debStr[0x7D][0] = "ADC $";  debStr[0x7D][1] = ",X";
    debStr[0x7E][0] = "ROR";
    debStr[0x7F][0] = "ILL_RRA abx";

    debStr[0x81][0] = "STA ($"; debStr[0x81][1] = ",X)";
    debStr[0x84][0] = "STY $";
    debStr[0x85][0] = "STA $";
    debStr[0x86][0] = "STX $";

    debStr[0x88][0] = "DEY";

    debStr[0x8A][0] = "TXA";

    debStr[0x8C][0] = "STY $";
    debStr[0x8D][0] = "STA $";
    debStr[0x8E][0] = "STA $";
    debStr[0x90][0] = "BCC";
    debStr[0x91][0] = "STA ($"; debStr[0x91][1] = "),Y";
    debStr[0x94][0] = "STY $"; debStr[0x94][1] = ",X";
    debStr[0x95][0] = "STA $"; debStr[0x95][1] = ",X";
    debStr[0x96][0] = "STX $"; debStr[0x96][1] = ",Y";

    debStr[0x98][0] = "TYA";
    debStr[0x99][0] = "STA $"; debStr[0x99][1] = ",Y";
    debStr[0x9A][0] = "TXS";

    debStr[0x9D][0] = "STA $"; debStr[0x9D][1] = ",X";




    debStr[0xA0][0] = "LDY #$";
    debStr[0xA1][0] = "LDA ($";  debStr[0xA1][1] = ", X)";
    debStr[0xA2][0] = "LDX #$";

    debStr[0xA4][0] = "LDY $";
    debStr[0xA5][0] = "LDA $";
    debStr[0xA6][0] = "LDX $";

    debStr[0xA8][0] = "TAY";
    debStr[0xA9][0] = "LDA #$";

    debStr[0xAA][0] = "TAX";

    debStr[0xAC][0] = "LDY $";
    debStr[0xAD][0] = "LDA $";
    debStr[0xAE][0] = "LDX $";

    debStr[0xB0][0] = "BCS";
    debStr[0xB1][0] = "LDA ($"; debStr[0xB1][1] = "), Y";

    debStr[0xB4][0] = "LDY $"; debStr[0xB4][1] = ",X";
    debStr[0xB5][0] = "LDA $"; debStr[0xB5][1] = ",X";
    debStr[0xB6][0] = "LDX $"; debStr[0xB6][1] = ",Y";
    debStr[0xB8][0] = "CLV";
    debStr[0xB9][0] = "LDA $"; debStr[0xB9][1] = ",Y";
    debStr[0xBA][0] = "TSX";

    debStr[0xBC][0] = "LDY $"; debStr[0xBC][1] = ",X";
    debStr[0xBD][0] = "LDA $"; debStr[0xBD][1] = ",X";
    debStr[0xBE][0] = "LDX $"; debStr[0xBE][1] = ",Y";

    debStr[0xC0][0] = "CPY #$";
    debStr[0xC1][0] = "CMP ($"; debStr[0xC1][1] = ",X)";

    debStr[0xC4][0] = "CPY $";
    debStr[0xC5][0] = "CMP $";
    debStr[0xC6][0] = "DEC $";
    debStr[0xC8][0] = "INY";
    debStr[0xC9][0] = "CMP #$";
    debStr[0xCA][0] = "DEX";
    debStr[0xCC][0] = "CPY $";
    debStr[0xCD][0] = "CMP $";
    debStr[0xCE][0] = "DEC $";


    debStr[0xD0][0] = "BNE";
    debStr[0xD1][0] = "CMP ($"; debStr[0xD1][1] = "),Y";
    debStr[0xD5][0] = "CMP $"; debStr[0xD5][1] = ",X";
    debStr[0xD6][0] = "DEC $";  debStr[0xD6][1] = ",X";
    debStr[0xD8][0] = "CLD";
    debStr[0xD9][0] = "CMP $";  debStr[0xD9][1] = ",Y";

    debStr[0xDD][0] = "CMP $";  debStr[0xDD][1] = ",X";
    debStr[0xDE][0] = "DEC $";  debStr[0xDE][1] = ",X";


    debStr[0xE0][0] = "CPX #$";

    debStr[0xE4][0] = "CPX $";

    debStr[0xE6][0] = "INC $";

    debStr[0xE8][0] = "INX";

    debStr[0xEA][0] = "NOP";

    debStr[0xEC][0] = "CPX $";

    debStr[0xEE][0] = "INC $";

    debStr[0xF0][0] = "BEQ";

    debStr[0xF6][0] = "INC $";  debStr[0xF6][1] = ",X";

    debStr[0xF8][0] = "SED";

    debStr[0xFE][0] = "INC $";  debStr[0xFE][1] = ",X";

#endif // CPU_DEBUG_INSTRUCTION_TEXT

    /* Set Instruction Cycle time */

    //LDA
    /*
    cycleTime[0xA9] = 2;
    cycleTime[0xAD] = 4;
    cycleTime[0xA5] = 3;
    cycleTime[0xA1] = 6;
    cycleTime[0xB1] = 5;
    cycleTime[0xB5] = 4;
    cycleTime[0xBD] = 4;
    cycleTime[0xB9] = 4;
    */



}

void Cpu::debug()
{
    if (debugEnabled)
    {
        cout << endl;
        cout << "*** Step Debug *** " << (int) cycles << endl;
        cout << "PC = " << hex << (int) pc << endl;
        cout << "Instruction " << setfill('0') << setw(2) << hex << (int) lastOpcode << " : " << debStr[lastOpcode][0] << " " << (int) debugValue << debStr[lastOpcode][1] << endl;
        cout << "Address : " << setfill('0') << setw(2) << hex << (int) debugValueAddress << "  Value : " << hex << (int) debugValue << endl;
        cout << "SREG: | N | V | R | B | D | I | Z | C |" << endl;
        cout << "      | " << (int) getNegativeFlag() << " | " << (int) getOverflowFlag() << " |   | " << (int) getBreakFlag() << " | " << (int) getDecimalFlag() << " | " << (int) getIRQFlag() << " | " << (int) getZeroFlag() << " | " << (int) getCarryFlag() << " |" << endl;
        cout << "A = " << setw(2) << hex << (int) a << "   X = " << setw(2) << hex << (int) x << "   Y = " << setw(2) << hex << (int) y << "   SP = " << setw(2) << hex << (int) sp << endl;

    }
}

uint8_t Cpu::fetchOpcode()
{
    return memory->read(pc);
}

unsigned int Cpu::getLastCycles()
{
    return cycles;
}

bool Cpu::executeOpcode()
{
    /* Prepare for opcode execution */
    boundaryCrossed = false;    //Reset in case a boundary cross is decisive
    cycles = 0;                 //Reset amount of cycles for this opcode

    /* Fetch opcode */
    uint8_t opcode = fetchOpcode();
    lastOpcode = opcode;

    if (pc == breakpoint || infiniteBranch == true)
    {
        cout << "PC = " << setfill('0') << setw(2) << hex << (int) pc << endl;
        return false;
    }

    if (debugPoint == pc || pc-1 == debugPoint || pc+1 ==debugPoint)
    {
        debugEnabled = true;
    }

    switch(opcode)
    {
        /* 0x0. */
        case 0x00:  triggerInterrupt(CPU_INTERRUPT_BRK);                cycles+=7;  break;
        case 0x01:  ORA(readIndirectX());   pc+=2;  cycles+=6;  break;

        case 0x05:  ORA(readZero());        pc+=2;  cycles+=2;  break;
        case 0x06:  writeZero(ASL(readZero())); pc+=2;  cycles+=5;  break;

        case 0x08:  push(sreg | 0x30);             pc+=1;  cycles+=3;  break;
        case 0x09:  ORA(readImmediate());   pc+=2;  cycles+=2;  break;
        case 0x0A:  a = ASL(a);             pc+=1;  cycles+=2;  break;

        case 0x0D:  ORA(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0x0E:  writeAbsolute(ASL(readAbsolute())); pc+=3;  cycles+=6;  break;

        /* 0x1. */
        case 0x10:  BPL();  break;
        case 0x11:  ORA(readIndirectY());   pc+=2;  cycles+=5;  break;

        case 0x15:  ORA(readZeroX());       pc+=2;  cycles+=3;  break;
        case 0x16:  writeZeroX(ASL(readZeroX()));   pc+=2;  cycles+=6;  break;


        case 0x18:  CLC();                  pc+=1;  cycles+=2;  break;
        case 0x19:  ORA(readAbsoluteY());   pc+=3;  cycles+=4;  break;

        case 0x1D:  ORA(readAbsoluteX());   pc+=3;  cycles+=4;  break;
        case 0x1E:  writeAbsoluteX(ASL(readAbsoluteX()));   pc+=3;  cycles+=7;  break;


        /* 0x2. */
        case 0x20:  JSR();                          cycles+=6;  break;
        case 0x21:  AND(readIndirectX());   pc+=2;  cycles+=6;  break;

        case 0x24:  BIT(readZero());        pc+=2;  cycles+=3;  break;
        case 0x25:  AND(readZero());        pc+=2;  cycles+=3;  break;
        case 0x26:  writeZero(ROL(readZero()));     pc+=2;  cycles+=5;  break;

        case 0x28:  sreg = pull();          pc+=1;  cycles+=4;  break;
        case 0x29:  AND(readImmediate());   pc+=2;  cycles+=2;  break;
        case 0x2A:  a = ROL(a);             pc+=1;  cycles+=2;  break;

        case 0x2C:  BIT(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0x2D:  AND(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0x2E:  writeAbsolute(ROL(readAbsolute())); pc+=3;  cycles+=6;  break;



        /* 0x3. */
        case 0x30:  BMI();  break;
        case 0x31:  AND(readIndirectY());   pc+=2;  cycles+=5;  break;

        case 0x35:  AND(readZeroX());       pc+=2;  cycles+=4;  break;
        case 0x36:  writeZeroX(ROL(readZeroX()));   pc+=2;  cycles+=6;  break;

        case 0x38:  SEC();                  pc+=1;  cycles+=2;  break;
        case 0x39:  AND(readAbsoluteY());   pc+=3;  cycles+=4;  break;

        case 0x3D:  AND(readAbsoluteX());   pc+=3;  cycles+=4;  break;
        case 0x3E:  writeAbsoluteX(ROL(readAbsoluteX()));   pc+=3;  cycles+=7;  break;


        /* 0x4. */
        case 0x40:  RTI();                          cycles+=6;  break;
        case 0x41:  EOR(readIndirectX());   pc+=2;  cycles+=6;  break;

        case 0x45:  EOR(readZero());        pc+=2;  cycles+=3;  break;
        case 0x46:  writeZero(LSR(readZero())); pc+=2;  cycles+=5;  break;

        case 0x48:  push(a);                pc+=1;  cycles+=3;  break;
        case 0x49:  EOR(readImmediate());   pc+=2;  cycles+=2;  break;
        case 0x4A:  a = LSR(a);             pc+=1;  cycles+=2;  break;



        case 0x4C:  JMP();                          cycles+=3;  break;
        case 0x4D:  EOR(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0x4E:  writeAbsolute(LSR(readAbsolute())); pc+=3;  cycles+=6;  break;



        /* 0x5. */
        case 0x50:  BVC();  break;
        case 0x51:  EOR(readIndirectY());   pc+=2;  cycles+=5;  break;

        case 0x55:  EOR(readZeroX());       cycles+=4;  pc+=2;  break;
        case 0x56:  writeZeroX(LSR(readZeroX()));   pc+=2;  cycles+=6;  break;

        case 0x58:  CLI();                  pc+=1;  cycles+=2;  break;
        case 0x59:  EOR(readAbsoluteY());   pc+=3;  cycles+=4;  break;


        case 0x5D:  EOR(readAbsoluteX());   pc+=3;  cycles+=4;  break;
        case 0x5E:  writeAbsoluteX(LSR(readAbsoluteX()));   pc+=3;  cycles+=7;  break;

        /* 0x6. */
        case 0x60:  RTS();                          cycles+=6;  break;
        case 0x61:  ADC(readIndirectX());   pc+=2;  cycles+=6;  break;


        case 0x65:  ADC(readZero());        pc+=2;  cycles+=3;  break;
        case 0x66:  writeZero(ROR(readZero())); pc+=2;  cycles+=5;  break;
        case 0x68:  /* PLA (pull accumulator) */
                    a = pull();
                    setZeroflagByValue(a);  setNegativeFlagByValue(a);
                    pc+=1;  cycles+=4;  break;
        case 0x69:  ADC(readImmediate());   pc+=2;  cycles+=2;  break;
        case 0x6A:  a = ROR(a);             pc+=1;  cycles+=2;  break;
        case 0x6C:  JMP_indirect();         cycles+=5;          break;
        case 0x6D:  ADC(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0x6E:  writeAbsolute(ROR(readAbsolute())); pc+=3;  cycles+=6;  break;




        /* 0x7. */
        case 0x70:  BVS();  break;
        case 0x71:  ADC(readIndirectY());   pc+=2;  cycles+=5;  break;
        case 0x75:  ADC(readZeroX());       pc+=2;  cycles+=4;  break;
        case 0x76:  writeZeroX(ROR(readZeroX()));   pc+=2;  cycles+=6;  break;


        case 0x78:  SEI();                  pc+=1;  cycles+=2;  break;
        case 0x79:  ADC(readAbsoluteY());   pc+=3;  cycles+=4;  break;
        case 0x7D:  ADC(readAbsoluteX());   pc+=3;  cycles+=4;  break;
        case 0x7E:  writeAbsoluteX(ROR(readAbsoluteX()));   pc+=3;  cycles+=7;  break;


        /* 0x8. */


        case 0x81:  writeIndirectX(STA());  pc+=2;  cycles+=6;  break;
        case 0x84:  writeZero(STY());       pc+=2;  cycles+=3;  break;
        case 0x85:  writeZero(STA());       pc+=2;  cycles+=3;  break;
        case 0x86:  writeZero(STX());       pc+=2;  cycles+=3;  break;
        case 0x88:  DEY();  break;

        case 0x8A:  TXA();  break;
        case 0x8C:  writeAbsolute(STY());   pc+=3;  cycles+=4;  break;
        case 0x8D:  writeAbsolute(STA());   pc+=3;  cycles+=4;  break;
        case 0x8E:  writeAbsolute(STX());   pc+=3;  cycles+=4;  break;


        /* 0x9. */
        case 0x90:  BCC();  break;
        case 0x91:  writeIndirectY(STA());  pc+=2;  cycles+=6;  break;
        case 0x94:  writeZeroX(STY());      pc+=2;  cycles+=4;  break;
        case 0x95:  writeZeroX(STA());      pc+=2;  cycles+=4;  break;
        case 0x96:  writeZeroY(STX());      pc+=2;  cycles+=4;  break;
        case 0x98:  TYA();  break;
        case 0x99:  writeAbsoluteY(STA());  pc+=3;  cycles+=5;  break;
        case 0x9A:  TXS();                  pc+=1;  cycles+=2;  break;
        case 0x9D:  writeAbsoluteX(STA());  pc+=3;  cycles+=5;  break;


        /* 0xA. */
        case 0xA0:  LDY(readImmediate());   pc+=2;  cycles+=2;  break;
        case 0xA1:  LDA(readIndirectX());   pc+=2;  cycles+=6;  break;
        case 0xA2:  LDX(readImmediate());   pc+=2;  cycles+=2;  break;

        case 0xA4:  LDY(readZero());        pc+=2;  cycles+=3;  break;
        case 0xA5:  LDA(readZero());        pc+=2;  cycles+=3;  break;
        case 0xA6:  LDX(readZero());        pc+=2;  cycles+=3;  break;

        case 0xA8:  TAY();  break;
        case 0xA9:  LDA(readImmediate());   pc+=2;  cycles+=2;  break;
        case 0xAA:  TAX();  break;
        case 0xAC:  LDY(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0xAD:  LDA(readAbsolute());    pc+=3;  cycles+=4;  break;
        case 0xAE:  LDX(readAbsolute());    pc+=3;  cycles+=4;  break;


        /* 0xB. */
        case 0xB0:  BCS();  break;
        case 0xB1:  LDA(readIndirectY());   pc+=2;  cycles+=5;  break;
        case 0xB4:  LDY(readZeroX());       pc+=2;  cycles+=4;  break;
        case 0xB5:  LDA(readZeroX());       pc+=2;  cycles+=4;  break;
        case 0xB6:  LDX(readZeroY());       pc+=2;  cycles+=4;  break;

        case 0xB8:  CLV();  pc+=1;  cycles+=2;  break;

        case 0xBD:  LDA(readAbsoluteX());   pc+=3;  cycles+=4;  break;
        case 0xB9:  LDA(readAbsoluteY());   pc+=3;  cycles+=4;  break;
        case 0xBA:  TSX();                  pc+=1;  cycles+=2;  break;

        case 0xBC:  LDY(readAbsoluteX());   pc+=3;  cycles+=4;  break;

        case 0xBE:  LDX(readAbsoluteY());   pc+=3;  cycles+=4;  break;

        /* 0xC. */
        case 0xC0:  compare(readImmediate(), y);    pc+=2;  cycles+=2;  break;
        case 0xC1:  compare(readIndirectX(), a);    pc+=2;  cycles+=6;  break;

        case 0xC4:  compare(readZero(), y); pc+=2;  cycles+=3;  break;
        case 0xC5:  compare(readZero(), a); pc+=2;  cycles+=3;  break;
        case 0xC6:  writeZero(DEC(readZero())); pc+=2;  cycles+=5;  break;

        case 0xC8:  INY();  break;
        case 0xC9:  compare(readImmediate(), a);    pc+=2;  cycles+=2;  break;
        case 0xCA:  DEX();  break;

        case 0xCC:  compare(readAbsolute(), y);     pc+=3;  cycles+=4;  break;
        case 0xCD:  compare(readAbsolute(), a);     pc+=3;  cycles+=4;  break;
        case 0xCE:  writeAbsolute(DEC(readAbsolute())); pc+=3;  cycles+=6;  break;

        /* 0xD. */
        case 0xD0:  BNE();  break;
        case 0xD1:  compare(readIndirectY(), a);    pc+=2;  cycles+=5;  break;

        case 0xD5:  compare(readZeroX(), a);    pc+=2;  cycles+=4;  break;
        case 0xD6:  writeZeroX(DEC(readZeroX()));    pc+=2;  cycles+=6;  break;
        case 0xD8:  CLD();                          pc+=1;  cycles+=2;  break;
        case 0xD9:  compare(readAbsoluteY(), a);    pc+=3;  cycles+=4;  break;

        case 0xDD:  compare(readAbsoluteX(), a);    pc+=3;  cycles+=4;  break;
        case 0xDE:  writeAbsoluteX(DEC(readAbsoluteX()));   pc+=3;  cycles+=7;  break;



        /* 0xE. */
        case 0xE0:  compare(readImmediate(), x);    pc+=2;  cycles+=2;  break;
        case 0xE1:  SBC(readIndirectX());           pc+=2;  cycles+=6;  break;
        case 0xE4:  compare(readZero(), x);         pc+=2;  cycles+=3;  break;
        case 0xE5:  SBC(readZero());                pc+=2;  cycles+=3;  break;
        case 0xE6:  writeZero(INC(readZero()));     pc+=2;  cycles+=3;  break;
        case 0xE8:  INX();  break;
        case 0xE9:  SBC(readImmediate());           pc+=2;  cycles+=2;  break;
        case 0xEA:  /* NOP */                       pc+=1;  cycles+=2;  break;

        case 0xEC:  compare(readAbsolute(), x);     pc+=3;  cycles+=4;  break;
        case 0xED:  SBC(readAbsolute());            pc+=3;  cycles+=4;  break;
        case 0xEE:  writeAbsolute(INC(readAbsolute())); pc+=3;  cycles+=6;  break;


        /* 0xF. */
        case 0xF0:  BEQ();  break;
        case 0xF1:  SBC(readIndirectY());           pc+=2;  cycles+=5;  break;
        case 0xF5:  SBC(readZeroX());               pc+=2;  cycles+=4;  break;
        case 0xF6:  writeZeroX(INC(readZeroX()));   pc+=2;  cycles+=6;  break;

        case 0xF8:  SED();  pc+=1;  cycles+=2;  break;
        case 0xF9:  SBC(readAbsoluteY());           pc+=3;  cycles+=4;  break;

        case 0xFD:  SBC(readAbsoluteX());           pc+=3;  cycles+=4;  break;  //TODO: boundary crossed not implemented in SBC instruction!

        case 0xFE:  writeAbsoluteX(INC(readAbsoluteX()));   pc+=3;  cycles+=7;  break;

        default:    cout << "Opcode not impelemented! " << hex << (int) opcode << " in " << hex << pc << endl; return false; //exit(1); break;



    }

    //memory->debugMemory(0x2000, 16);
    debug();

    if (pc == 0x3500 && a == 0x7f)
    {
        //return false;
    }
    return true;
}


/** Implementation of instruction set */
/* Load instructions */
void Cpu::LDA(uint8_t value)
{
    a = value;
    setZeroflagByValue(a);
    setNegativeFlagByValue(a);
    if (boundaryCrossed == true)
    {
        cycles++;        //LDA takes one clock cycle more in case of a boundary cross in indexed mode
    }
}

void Cpu::LDX(uint8_t value)
{
    x = value;
    setZeroflagByValue(x);
    setNegativeFlagByValue(x);
    if (boundaryCrossed == true)
    {
        cycles++;       //see LDA boundary crossed
    }
}

void Cpu::LDY(uint8_t value)
{
    y = value;
    setZeroflagByValue(y);
    setNegativeFlagByValue(y);
    if (boundaryCrossed == true)
    {
        cycles++;       //see LDA boundary crossed
    }
}

/* Store instructions */
uint8_t Cpu::STA()
{
    return a;
}

uint8_t Cpu::STX()
{
    return x;
}

uint8_t Cpu::STY()
{
    return y;
}

/* Jump instructions */
void Cpu::JMP()
{
    // Fetch absolute Address
    uint16_t address = memory->read(pc + 1) | (memory->read(pc + 2) << 8);

    // Set programm counter to address
    pc = address;
}

void Cpu::JMP_indirect()
{
    // Fetch indirect address
    uint16_t indirectAddress = memory->read(pc + 1) | (memory->read(pc + 2) << 8);

    uint16_t address;

    /* Check for JMP bug */
    if ((indirectAddress & 0xFF) == 0xFF)
    {
        // JMP with Page bug
        address = memory->read(indirectAddress) | (memory->read(indirectAddress - 0xFF) << 8);
    }
    else
    {
        address = memory->read(indirectAddress) | (memory->read(indirectAddress + 1) << 8);
    }

    pc = address;
}

/* Branches */
void Cpu::BPL()
{
    cycles+=2;
    if (getNegativeFlag() == false)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BMI()
{
    cycles+=2;
    if (getNegativeFlag() == true)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BVC()
{
    cycles+=2;
    if (getOverflowFlag() == false)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BVS()
{
    cycles+=2;
    if (getOverflowFlag() == true)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BCC()
{
    cycles+=2;
    if (getCarryFlag() == false)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BCS()
{
    cycles+=2;
    if (getCarryFlag() == true)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BNE()
{
    cycles+=2;
    if (getZeroFlag() == false)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }
}

void Cpu::BEQ()
{
    cycles+=2;
    if (getZeroFlag() == true)
    {
        relativeBranch();
    }
    else
    {
        pc+=2;
    }

}

void Cpu::relativeBranch()
{
    signed char branchValue = memory->read(pc + 1);
    uint16_t pc_old = pc;

    pc++;
    pc += branchValue;

    debugValue = branchValue;
    if ((uint8_t) branchValue == 0xFE)
    {
        infiniteBranch = true;
    }

    cycles++;
    //cout << "Branch value : " << branchValue << endl;


    if ((pc & 0xFF00) != (pc_old & 0xFF00))
    {
        cycles++;
    }

    pc++;
}


/* Status register instruction */
void Cpu::CLC()
{
    setCarryFlag(false);
}
void Cpu::SEC()
{
    setCarryFlag(true);
}
void Cpu::CLI()
{
    setIRQFlag(false);
}
void Cpu::SEI()
{
    setIRQFlag(true);
}
void Cpu::CLV()
{
    setOverflowFlag(false);
}
void Cpu::CLD()
{
    setDecimalFlag(false);
}
void Cpu::SED()
{
    setDecimalFlag(true);
}

/* Arithmetic instructions */
void Cpu::ADC(uint8_t value)
{
    if(getDecimalFlag() == false)
    {
        uint8_t ovf_m = a;
        uint8_t ovf_n = value;

        uint16_t adc_result = a + value;

        if (getCarryFlag() == true)
        {
            adc_result++;
        }

        if ((adc_result & 0x0100) > 0)
        {
            setCarryFlag(true);
        }
        else
        {
            setCarryFlag(false);
        }

        // Calculate overflow flag
        uint8_t result = adc_result & 0xFF;
        setOverflowFlag(((ovf_m ^ result) & (ovf_n ^ result) & 0x80) != 0);
        a = result;
        setNegativeFlagByValue(a);
        setZeroflagByValue(a);
    }
    else
    {
        //debugEnabled = true;
        //cout << "Decimal test support!" << endl;
        //cout << "A=" << (int) a << "  B=" << (int) value;

        unsigned int tmp_value = value;
        unsigned int tmp;
        unsigned int tmp2;

        tmp = (a & 0x0F) + (tmp_value & 0x0F) + (int) getCarryFlag();
        tmp2 = (a & 0xF0) + (tmp_value & 0xF0);
        if (tmp > 9)
        {
            tmp2+=0x10;
            tmp +=6;
        }
        setOverflowFlag(~(a ^ tmp_value) & (a ^ tmp2) & 0x80);
        if (tmp2 > 0x90)
        {
            tmp2 +=0x60;
        }
        setCarryFlag(tmp2 & 0xFF00);
        tmp = (tmp & 0x0F) + (tmp2 & 0xF0);
        setNegativeFlagByValue(tmp);
        setZeroflagByValue(tmp);
        a = tmp;
    }



    if (boundaryCrossed == true)
    {
        cycles+=1;
    }


}

void Cpu::SBC(uint8_t value)
{

    if (getDecimalFlag() == false)
    {
        /* Binary mode subtraction */
        //cout << "Subtract debug **** Binary-Mode" << endl;
        //cout << "PC = " << setw(4) << hex << (int) pc << endl;
        //cout << "A = " << setw(2) << hex << (int) a << "  V = " << setw(2) << hex << (int) value << endl;
        int ovf_m = a;
        int ovf_n = value;

        unsigned short tmp = a - value + (int) getCarryFlag() -1;

        uint16_t sbc_result = a + (value ^ 0xFF) + (int) getCarryFlag();

        //setCarryFlag(!(a < value));   //New Carryflag obviously faulty
        setCarryFlag(a + (int) getCarryFlag() - 1 >= value);    //New implementation from WinVice

        uint8_t result = sbc_result & 0xFF;

        /* Calculation of OVF-Flag taken from 65C02core.c from the Vice emulator project */

        setOverflowFlag(((a ^ tmp) & 0x80) && ((a ^ value) & 0x80));
        a = result;
        setNegativeFlagByValue(a);
        setZeroflagByValue(a);
    }
    else
    {
        /* Decimal mode subtraction */
        //debugEnabled = true;

        unsigned short tmp2;
        unsigned short src = value;
        unsigned short tmp = a - src + (int) getCarryFlag() - 1;
        setOverflowFlag(((a ^ tmp) & 0x80) && ((a ^ src) & 0x80));
        if (tmp > 0xFF)
        {
            tmp -= 0x60;
        }
        tmp2 = (a & 0x0F) - (src & 0x0F) + (int) getCarryFlag() - 1;
        if (tmp2 > 0xFF)
        {
            tmp -= 6;
        }

        setCarryFlag(a + (int) getCarryFlag() - 1 >= src);

        setNegativeFlagByValue(tmp & 0xFF);
        setZeroflagByValue(a & 0xFF);

        a = 0xFF & tmp;
    }

    if (boundaryCrossed == true)
    {
        cycles+=1;
    }


}

void Cpu::AND(uint8_t value)
{
    a &= value;
    setZeroflagByValue(a);
    setNegativeFlagByValue(a);
    if (boundaryCrossed == true)
    {
        cycles++;
    }
}

void Cpu::ORA(uint8_t value)
{
    a |= value;
    setZeroflagByValue(a);
    setNegativeFlagByValue(a);
    if (boundaryCrossed == true)
    {
        cycles++;
    }
}

void Cpu::EOR(uint8_t value)
{
    a ^= value;
    setZeroflagByValue(a);
    setNegativeFlagByValue(a);
    if (boundaryCrossed == true)
    {
        cycles++;
    }
}

/* Bitshift/rotation */
uint8_t Cpu::ROL(uint8_t value)
{
    /* Store carry to shift it in later */
    bool carryOld = getCarryFlag();

    /* Set new carry flag value by MSb of value */
    setCarryFlag((value & 0x80) == 0x80);

    /* Shift bits left by 1 position */
    uint8_t result = value << 1;

    /* Add Carry flag to LSb */
    if (carryOld == true)
    {
        result |= 0x01;
    }

    /* Set status register */
    setZeroflagByValue(result);
    setNegativeFlagByValue(result);

    return result;
}

uint8_t Cpu::ROR(uint8_t value)
{
    /* Store carry to shift in later */
    bool carryOld = getCarryFlag();

    /* Set Carry to new value */
    setCarryFlag((bool) (0x01 & value));

    /* Shift right */
    uint8_t result = value >> 1;

    /* Add Carry on the left of result */
    if (carryOld == true)
    {
        result |= 0x80;
    }

    /* Set status register */
    setZeroflagByValue(result);
    setNegativeFlagByValue(result);

    return result;
}

uint8_t Cpu::ASL(uint8_t value)
{
    /* Set Carry flag to MSb of value */
    setCarryFlag((value & 0x80) == 0x80);

    /* Shift value */
    uint8_t result = value << 1;

    /* Set status register flags */
    setZeroflagByValue(result);
    setNegativeFlagByValue(result);

    return result;
}

uint8_t Cpu::LSR(uint8_t value)
{
    /* Set Carry flag to LSb of value */
    setCarryFlag((bool) (value & 0x01));

    /* Shift value */
    uint8_t result = value >> 1;

    /* Set status register flags */
    setZeroflagByValue(result);
    setNegativeFlagByValue(result);

    return result;
}

uint8_t Cpu::INC(uint8_t value)
{
    uint8_t result = value + 1;
    setZeroflagByValue(result);
    setNegativeFlagByValue(result);
    return result;
}

uint8_t Cpu::DEC(uint8_t value)
{
    uint8_t result = value - 1;
    setZeroflagByValue(result);
    setNegativeFlagByValue(result);
    return result;
}

/* Bit instruction */
void Cpu::BIT(uint8_t value)
{
    setZeroflagByValue(a & value);
    setNegativeFlagByValue(value);
    if ((value & 0x40) == 0x40)
    {
        setOverflowFlag(true);
    }
    else
    {
        setOverflowFlag(false);
    }
}

/* Register instructions */
void Cpu::TAX()
{
    pc+=1;
    cycles+=2;
    x = a;
    setNegativeFlagByValue(x);
    setZeroflagByValue(x);
}

void Cpu::TXA()
{
    pc+=1;
    cycles+=2;
    a = x;
    setNegativeFlagByValue(a);
    setZeroflagByValue(a);
}

void Cpu::DEX()
{
    pc+=1;
    cycles+=2;
    x--;
    setZeroflagByValue(x);
    setNegativeFlagByValue(x);
}
void Cpu::INX()
{
    pc+=1;
    cycles+=2;
    x++;
    setZeroflagByValue(x);
    setNegativeFlagByValue(x);
}

void Cpu::TAY()
{
    pc+=1;
    cycles+=2;
    y = a;
    setZeroflagByValue(y);
    setNegativeFlagByValue(y);
}

void Cpu::TYA()
{
    pc+=1;
    cycles+=2;
    a = y;
    setZeroflagByValue(a);
    setNegativeFlagByValue(a);
}

void Cpu::DEY()
{
    pc+=1;
    cycles+=2;
    y--;
    setZeroflagByValue(y);
    setNegativeFlagByValue(y);
}

void Cpu::INY()
{
    pc+=1;
    cycles+=2;
    y++;
    setZeroflagByValue(y);
    setNegativeFlagByValue(y);
}

/* Stack instructions */
void Cpu::TXS()
{
    sp = x;
}

void Cpu::TSX()
{
    x = sp;
    setNegativeFlagByValue(x);
    setZeroflagByValue(x);
}

void Cpu::push(uint8_t reg_value)
{
    memory->write((0x0100 | sp), reg_value);
    sp--;
    /*
    if (debugEnabled)
    {
        cout << "Stack:    ****************" << endl;
        memory->debugMemory(0x100,256);
        cout << "--- end stack ------------" << endl;
    }
    */
}

uint8_t Cpu::pull()
{
    sp++;
    return memory->read(0x0100 | sp);
}

void Cpu::JSR()
{
    /* Read new PC value from memory */
    uint16_t newPC = memory->read(pc + 1) | (memory->read(pc + 2) << 8);

    /* Push PC onto stack */
    pc += 2;
    push(0xFF & (pc >> 8));
    push(0xFF & pc);

    pc = newPC;
}

void Cpu::RTS()
{
    /* Pull PC-1 from stack */
    uint8_t pcLow  = pull();
    uint8_t pcHigh = pull();

    pc = ((pcHigh << 8) | pcLow) + 1;
}

void Cpu::RTI()
{
    /* Pull S-Reg from stack */
    sreg = pull();

    /* Pull actual PC from Stack */
    uint8_t pcLow  = pull();
    uint8_t pcHigh = pull();

    pc = (pcHigh << 8) | pcLow;
}

/* Compare instructions */
void Cpu::compare(uint8_t value, uint8_t reg)
{
    if (debugEnabled)
    {
        cout << "Compare values: " << endl;
        cout << "value = 0x" << setfill('0') << setw(2) << hex << (int) value << endl;
        cout << "regi. = 0x" << setfill('0') << setw(2) << (int) reg << endl;
    }
    if (reg < value)
    {
        setNegativeFlagByValue((uint8_t) (reg - value) & 0xFF);
        setZeroFlag(false);
        setCarryFlag(false);
    }
    else if (reg == value)
    {
        setNegativeFlag(false);
        setZeroFlag(true);
        setCarryFlag(true);
    }
    else if (reg > value)
    {
        setNegativeFlagByValue((uint8_t) (reg - value) & 0xFF);
        setZeroFlag(false);
        setCarryFlag(true);
    }
    else
    {
        cout << "Comparison error!" << endl;
        exit(0);
    }

    if (boundaryCrossed == true)
    {
        cycles++;
    }
}

/* Interrupts */
void Cpu::triggerInterrupt(uint8_t interrupt_type)
{
//TODO: Check for perfect implementation
    //Push PC and Status-register onto stack
    push(((pc+2) >> 8) & 0xFF);
    push((pc+2) & 0xFF);

    /* Read Interrupt vector */
    uint16_t intVector_offset;
    if (interrupt_type == CPU_INTERRUPT_NMI)
    {
        /* NMI vector is 0xFFFA-FFFB */
        intVector_offset = 0xFFFA;
        push(sreg | 0x20);
    }
    else if (interrupt_type == CPU_INTERRUPT_IRQ)
    {
        /* IRQ Vector is 0xFFFE-FFFF */
        intVector_offset = 0xFFFE;
        push(sreg | 0x20);
    }
    else if (interrupt_type == CPU_INTERRUPT_BRK)
    {
        /* BRK Vector is same as IRQ vect. 0xFFFE-FFFF */
        intVector_offset = 0xFFFE;
        push(sreg | 0x30);
        setIRQFlag(true);
    }

    uint8_t pcLow  = memory->read(intVector_offset);
    uint8_t pcHigh = memory->read(intVector_offset + 1);

    pc = (pcHigh << 8) | pcLow;

    cycles+=7;
}

/** Implementation of addressing modes */
/* Read mode */
uint8_t Cpu::readImmediate()
{
    uint8_t value = memory->read(pc + 1);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = pc + 1;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readAbsolute()
{
    uint16_t address = memory->read(pc + 1) | (memory->read(pc + 2) << 8);
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readAbsoluteX()
{
    boundaryCrossed = false;
    uint16_t baseAddress = memory->read(pc + 1) | (memory->read(pc + 2) << 8);
    if ( (uint16_t) (baseAddress & 0xFF) + (uint16_t) x > 0xFF )
    {
        boundaryCrossed = true;
    }
    uint16_t address = baseAddress + x;
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readAbsoluteY()
{
    boundaryCrossed = false;
    uint16_t baseAddress = memory->read(pc + 1) | (memory->read(pc + 2) << 8);
    if ((uint16_t) (baseAddress & 0xFF) + (uint16_t) y > 0xFF)
    {
        boundaryCrossed = true;
    }
    uint16_t address = baseAddress + y;
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readZero()
{
    uint16_t address = memory->read(pc + 1) & 0x00FF;
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readZeroX()
{
    uint16_t address = (memory->read(pc + 1) + x) & 0x00FF;
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readZeroY()
{
    uint16_t address = (memory->read(pc + 1) + y) & 0x00FF;
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readIndirectX()
{
    uint16_t zeroAddress = (memory->read(pc + 1) + x) & 0x00FF;
    uint16_t address = (memory->read(zeroAddress) | (memory->read(zeroAddress + 1) << 8));
    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

uint8_t Cpu::readIndirectY()
{
    boundaryCrossed = false;
    uint16_t zeroAddress = memory->read(pc + 1);
    uint16_t baseAddress = (memory->read(zeroAddress) | (memory->read(zeroAddress + 1) << 8));
    if ((uint16_t) (baseAddress & 0xFF) + (uint16_t) y > 0xFF)
    {
        boundaryCrossed = true;
    }
    uint16_t address = baseAddress + y;

    uint8_t value = memory->read(address);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return value;
}

/* Write modes */
void Cpu::writeAbsolute(uint8_t value)
{

    uint16_t address = memory->read(pc + 1) | (memory->read(pc + 2) << 8);

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeZero(uint8_t value)
{
    uint16_t address = memory->read(pc + 1);

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeAbsoluteX(uint8_t value)
{
    uint16_t address = (memory->read(pc + 1) | (memory->read(pc + 2) << 8)) + x;

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeAbsoluteY(uint8_t value)
{
    uint16_t address = (memory->read(pc + 1) | (memory->read(pc + 2) << 8)) + y;

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeZeroX(uint8_t value)
{
    uint16_t address = (memory->read(pc + 1) + x) & 0x00FF;

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeZeroY(uint8_t value)
{
    uint16_t address = (memory->read(pc + 1) + y) & 0x00FF;

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeIndirectX(uint8_t value)
{
    uint16_t zeroAddress = (memory->read(pc + 1) + x) & 0xFF;

    uint16_t address = memory->read(zeroAddress) | (memory->read(zeroAddress + 1) << 8);

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}

void Cpu::writeIndirectY(uint8_t value)
{
    uint16_t zeroAddress = memory->read(pc + 1) & 0xFF;

    uint16_t address = (uint16_t) (memory->read(zeroAddress) | (memory->read(zeroAddress + 1) << 8)) + y;

    memory->write(address, value);
#ifdef CPU_DEBUG_INSTRUCTION_TEXT
    debugValueAddress = address;
    debugValue = value;
#endif // CPU_DEBUG_INSTRUCTION_TEXT
    return;
}


/** Status register methods */
void Cpu::setZeroflagByValue(uint8_t value)
{
    if (value == 0)
    {
        setZeroFlag(true);
    }
    else
    {
        setZeroFlag(false);
    }
}

void Cpu::setNegativeFlagByValue(uint8_t value)
{
    if ( (value & 0b10000000) > 0 )
    {
        setNegativeFlag(true);
    }
    else
    {
        setNegativeFlag(false);
    }
}

bool Cpu::getZeroFlag()
{
    return (sreg & 0b00000010) >> 1;
}
bool Cpu::getCarryFlag()
{
    return (sreg & 0b00000001) >> 0;
}
bool Cpu::getIRQFlag()
{
    return (sreg & 0b00000100) >> 2;
}
bool Cpu::getDecimalFlag()
{
    return (sreg & 0b00001000) >> 3;
}
bool Cpu::getBreakFlag()
{
    return (sreg & 0b00010000) >> 4;
}
bool Cpu::getOverflowFlag()
{
    return (sreg & 0b01000000) >> 6;
}
bool Cpu::getNegativeFlag()
{
    return (sreg & 0b10000000) >> 7;
}

// Support method:

void Cpu::setSregBit(const uint8_t bit, bool value)
{
    if (value == true)
    {
        sreg |= (1 << bit);
    }
    else
    {
        sreg &= ~(1 << bit);
    }
}

void Cpu::setZeroFlag(bool value)
{
    setSregBit(1, value);
}

void Cpu::setCarryFlag(bool value)
{
    setSregBit(0, value);
}
void Cpu::setIRQFlag(bool value)
{
    setSregBit(2, value);
}
void Cpu::setDecimalFlag(bool value)
{
    setSregBit(3, value);
}
void Cpu::setBreakFlag(bool value)
{
    setSregBit(4, value);
}
void Cpu::setOverflowFlag(bool value)
{
    setSregBit(6, value);
}
void Cpu::setNegativeFlag(bool value)
{
    setSregBit(7, value);
}

