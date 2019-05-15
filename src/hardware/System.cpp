#include "System.h"

#include <fstream>

System::System()
{
    _mmu = new Mmu();
}

System::~System()
{
    delete _mmu;
}

void System::bootstrap()
{
    // load the system rom inside the Mmu (inside the first 256 bytes)
    std::ifstream systemRomFile("DMG_ROM.bin");
    systemRomFile.read((char *) &_mmu->_memory[0], 256);

    // // ( DBG: dump the content of the system rom
    // for (int i = 0; i < 256; i++)
    //     printf("0x%02x\n", _mmu->_memory[i]);
    // // )
}