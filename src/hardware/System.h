#ifndef SYSTEM_H
#define SYSTEM_H

#include "Mmu.h"

class System
{
    private:
        Mmu *_mmu;

    public:
        System();
        ~System();

        void bootstrap();
};

#endif