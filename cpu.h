#ifndef CPU_H
#define CPU_H

#include <iostream>

#include "memory.h"

using namespace std;





class Cpu {
private:
    uint8_t a;      //Accumulator
    uint8_t x;      //X-Register
    uint8_t y;      //Y-Register
    uint8_t sp;     //Stackpointer (lower 8 bits)

    uint16_t pc;    //Programmcounter

    uint8_t sreg;   //Status register

    /* Instruction methods declaration */
    // Load instructions
    void LDA(uint8_t value);
    void LDX(uint8_t value);
    void LDY(uint8_t value);

    // Store instructions
    uint8_t STA();
    uint8_t STX();
    uint8_t STY();

    // Jump instructions
    void JMP();
    void JMP_indirect();

    // Branch instructions
    void BPL();
    void BMI();
    void BVC();
    void BVS();
    void BCC();
    void BCS();
    void BNE();
    void BEQ();
    void relativeBranch();

    // Flag instructions
    void CLC();
    void SEC();
    void CLI();
    void SEI();
    void CLV();
    void CLD();
    void SED();

    /* Arithmetic instructions */
    void ADC(uint8_t value);
    void SBC(uint8_t value);

    void AND(uint8_t value);
    void ORA(uint8_t value);
    void EOR(uint8_t value);

    /* Bitshift/rotation */
    uint8_t ROL(uint8_t value);
    uint8_t ROR(uint8_t value);

    uint8_t ASL(uint8_t value);
    uint8_t LSR(uint8_t value);

    uint8_t INC(uint8_t value);
    uint8_t DEC(uint8_t value);

    /* Compare instructions */
    void compare(uint8_t value, uint8_t reg);

    /* Register instructions */
    void TAX();
    void TXA();
    void DEX();
    void INX();
    void TAY();
    void TYA();
    void DEY();
    void INY();

    /* Stack instructions */
    void TXS();
    void TSX();
    void push(uint8_t reg_value);
    uint8_t pull();

    void PHP();
    void PLP();

    void JSR();
    void RTS();
    void RTI();

    /* Bit Instruction */
    void BIT(uint8_t value);




    /* Addressing mode declarations */
    // Read mode
    uint8_t readImmediate();        // LDA #$10
    uint8_t readAbsolute();         // LDA $8000
    uint8_t readZero();     // LDA $80
    uint8_t readIndirectX();        // LDA ($80,X)
    uint8_t readIndirectY();        // LDA ($80), Y
    uint8_t readZeroX();            // LDA $80, X
    uint8_t readZeroY();            // LDA $80, Y
    uint8_t readAbsoluteX();        // LDA $8000, X
    uint8_t readAbsoluteY();        // LDA $8000, Y

    // Write mode
    void writeAbsolute(uint8_t value);          // STA $8000
    void writeZero(uint8_t value);              // STA $80
    void writeIndirectX(uint8_t value);         // STA ($80, X)
    void writeIndirectY(uint8_t value);         // STA ($80), Y
    void writeZeroX(uint8_t value);             // STA ($80), X
    void writeZeroY(uint8_t value);
    void writeAbsoluteX(uint8_t value);
    void writeAbsoluteY(uint8_t value);

    /* Status register methods */
    void setZeroflagByValue(uint8_t value);
    void setNegativeFlagByValue(uint8_t value);

    void setSregBit(const uint8_t bit, bool value);
    bool getZeroFlag();
    bool getCarryFlag();
    bool getIRQFlag();
    bool getDecimalFlag();
    bool getBreakFlag();
    bool getOverflowFlag();
    bool getNegativeFlag();

    void setZeroFlag(bool value);
    void setCarryFlag(bool value);
    void setIRQFlag(bool value);
    void setDecimalFlag(bool value);
    void setBreakFlag(bool value);
    void setOverflowFlag(bool value);
    void setNegativeFlag(bool value);


    string debStr[256][2]; //Debug instruction string
    uint8_t cycleTime[256];
    void initializeTable();


    uint8_t fetchOpcode();

    Memory *memory;

    uint16_t debugValueAddress;
    uint8_t debugValue;

    bool boundaryCrossed;

    unsigned int cycles;


public:
    Cpu();
    void attachMemory(Memory* mem);
    void reset(uint16_t PC_position);
    bool executeOpcode();
    unsigned int getLastCycles();

    #define CPU_INTERRUPT_NMI   1
    #define CPU_INTERRUPT_IRQ   2
    #define CPU_INTERRUPT_BRK   3

    void triggerInterrupt(uint8_t interrupt_type);


    void debug();

};

#endif // CPU_H

