#ifndef CPU_H
#define CPU_H

#include "Mmu.h"

class Cpu
{
    private:
        Mmu *_mmu;

        // Buffer containing the state of all registers
        uint8_t _state[12];

        // Registers offsets in the _state buffer
        enum {
            // 8 bit regs (AF, BC, DE, HL, are sometimes used as 16 bit regs)
            R_A = 1, R_F = 0,
            R_B = 3, R_C = 2,        
            R_D = 5, R_E = 4,
            R_H = 7, R_L = 6,

            // 16 bit regs
            R_SP = 8,
            R_PC = 10,
        };

        // Flag bit masks (the lower nibble in F is never used)
        enum {            
            F_C = 0x10, // Carry Flag (C):
                        // This bit is set if a carry occurred from the last
                        // math operation or if register A is the smaller value
                        // when executing the CP instruction

            F_H = 0x20, // Half Carry Flag (H):
                        // This bit is set if a carry occurred from the lower
                        // nibble in the last math operation

            F_N = 0x40, // Subtract Flag (N):
                        // This bit is set if a subtraction was performed in the
                        // last math instruction

            F_Z = 0x80, // Zero Flag (Z):
                        // This bit is set when the result of a math operation
                        // is zero or two values match when using the CP
                        // instruction
        };

        // filled during the fetch phase
        uint16_t _opcode;

        bool _isHalted;


        // returns true if the condition enconded in the _opcode is met
        bool _checkCondition();        

        // reads a byte and increments the PC by 1
        inline uint8_t _readb_PC();

        // reads a word and increments the PC by 2
        inline uint16_t _readw_PC();

        // push a byte onto the stack
        inline void _pushb(uint8_t b);

        // push a word onto the stack
        inline void _pushw(uint16_t w);

        // pop a byte from the stack
        inline uint8_t _popb();

        // pop a word from the stack
        inline uint16_t _popw();

    public:
        Cpu(Mmu *mmu);

        void reset();

        void fetch();
        void execute();

        void dump();

        bool cycle();
};

#endif