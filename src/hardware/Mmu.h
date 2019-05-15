#ifndef MMU_H
#define MMU_H

#include <stdint.h>

class Mmu
{
    private:
        
        
    public:
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
        uint8_t _memory[65536];

        Mmu();

        uint8_t readb(uint16_t addr);
        uint16_t readw(uint16_t addr);

        void writeb(uint16_t addr, uint8_t val);
        void writew(uint16_t addr, uint16_t val);
};

#endif