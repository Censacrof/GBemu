#ifndef CPU_H
#define CPU_H

#include "Mmu.h"

class Cpu
{
    private:
        Mmu *_mmu;

        // Buffer containing the state of all registers
        uint8_t _state[96];

        // Registers offsets in the _state buffer
        enum {
            // 8 bit regs (AF, BC, DE, HL, are sometimes used as 16 bit regs)
            R_A =  8, R_F =  0,
            R_B = 24, R_C = 16,            
            R_D = 40, R_E = 32,
            R_H = 56, R_L = 48,

            // 16 bit regs
            R_SP = 64,
            R_PC = 80,       
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

    public:
        Cpu(Mmu *mmu);

        void reset();
};

#endif