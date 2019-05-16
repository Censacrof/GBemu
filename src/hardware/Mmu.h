#ifndef MMU_H
#define MMU_H

#include <stdint.h>

class Mmu
{
    private:
        
    public:
        enum {
            TIMA = 0xFF05,
            TMA  = 0xFF06,
            TAC  = 0xFF07,
            NR10 = 0xFF10,
            NR11 = 0xFF11,
            NR12 = 0xFF12,
            NR14 = 0xFF14,
            NR21 = 0xFF16,
            NR22 = 0xFF17,
            NR24 = 0xFF19,
            NR30 = 0xFF1A,
            NR31 = 0xFF1B,
            NR32 = 0xFF1C,
            NR33 = 0xFF1E,
            NR41 = 0xFF20,
            NR42 = 0xFF21,
            NR43 = 0xFF22,
            // NR30 = 0xFF23,
            NR50 = 0xFF24,
            NR51 = 0xFF25,
            NR52 = 0xFF26,
            LCDC = 0xFF40,
            SCY  = 0xFF42,
            SCX  = 0xFF43,
            LYC  = 0xFF45,
            BGP  = 0xFF47,
            OBP0 = 0xFF48,
            OBP1 = 0xFF49,
            WY   = 0xFF4A,
            WX   = 0xFF4B,
            IE   = 0xFFFF,
        };

        static const uint32_t ADDRESSABLE_SPACE_SIZE = 65536;
        // Interrupt Enable Register
        // --------------------------- FFFF
        // Internal RAM
        // --------------------------- FF80
        // Empty but unusable for I/O
        // --------------------------- FF4C
        // I/O ports
        // --------------------------- FF00
        // Empty but unusable for I/O
        // --------------------------- FEA0
        // Sprite Attrib Memory (OAM)
        // --------------------------- FE00
        // Echo of 8kB Internal RAM
        // --------------------------- E000
        // 8kB Internal RAM
        // --------------------------- C000
        // 8kB switchable RAM bank
        // --------------------------- A000
        // 8kB Video RAM
        // --------------------------- 8000 --
        // 16kB switchable ROM bank |
        // --------------------------- 4000 |= 32kB Cartrigbe
        // 16kB ROM bank #0 |
        // --------------------------- 0000 --
        uint8_t _memory[ADDRESSABLE_SPACE_SIZE];

        Mmu();

        uint8_t readb(uint16_t addr);
        uint16_t readw(uint16_t addr);

        void writeb(uint16_t addr, uint8_t val);
        void writew(uint16_t addr, uint16_t val);
};

#endif