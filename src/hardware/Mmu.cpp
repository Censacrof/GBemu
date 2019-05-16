#include "Mmu.h"

Mmu::Mmu()
{
    
}

uint8_t Mmu::readb(uint16_t addr)
{
    return _memory[addr];
}

uint16_t Mmu::readw(uint16_t addr)
{
    return *((uint16_t *) &_memory[addr]);
}

void Mmu::writeb(uint16_t addr, uint8_t val)
{
    _memory[addr] = val;
}

void Mmu::writew(uint16_t addr, uint16_t val)
{
    *((uint16_t *) &_memory[addr]) = val;
}