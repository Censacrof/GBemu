#ifndef SYSTEM_H
#define SYSTEM_H

#include "Mmu.h"
#include "Cpu.h"

class System
{
    private:
        Mmu *_mmu;
        Cpu *_cpu;

    public:
        System();
        ~System();

        void bootstrap();
        void run();
};

#endif