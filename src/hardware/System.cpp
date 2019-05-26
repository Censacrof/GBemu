#include "System.h"

#include <fstream>

System::System()
{
    _mmu = new Mmu();
    _cpu = new Cpu(_mmu);
}

System::~System()
{
    delete _mmu;
    delete _cpu;
}

void System::bootstrap()
{
    // load the system rom inside the Mmu (inside the first 256 bytes)
    std::ifstream systemRomFile("DMG_ROM.bin");
    systemRomFile.read((char *) &_mmu->_memory[0], 256);


    // // ( DBG
    // _mmu->_memory[0] = 0xcb;
    // _mmu->_memory[1] = 0x06;
    // // )

    // // ( DBG: dump the content of the system rom
    // for (int i = 0; i < 4; i++)
    //     printf("0x%02x\n", _mmu->_memory[i]);
    // // )    
}


void System::run()
{
    _cpu->reset();

    int i = 0;// DBG
    while (_cpu->cycle())
    {
        _cpu->dump();

        if (i++ >= 10)
            break;
    }
}