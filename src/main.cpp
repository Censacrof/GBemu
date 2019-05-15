#include <iostream>

#include "hardware/System.h"

uint8_t buffer[256];

int main(int argc, char *argv[])
{
    std::cout << "Hello World" << std::endl;
    
    System sys;
    sys.bootstrap();    

    return 0;    
}