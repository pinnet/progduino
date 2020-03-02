#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <Arduino.h>

class MemoryManager {



public: MemoryManager(void);
        int freeMemory(void);
        bool initialiseRAM(int);



};

#endif