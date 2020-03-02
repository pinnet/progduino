#include "MemoryManager.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

MemoryManager::MemoryManager() {
    
}

int MemoryManager::freeMemory() {
    char top;
    #ifdef __arm__
    return &top - reinterpret_cast<char*>(sbrk(0));
    #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
    #else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
    #endif  // __arm__
}
boolean MemoryManager::initialiseRAM(int bytes){
    byte *ptr = (byte *)calloc(bytes,sizeof(byte)); 
    if(ptr == NULL) return false;
    int start_mem = MemoryManager::freeMemory();
    delay(1000);   
    free(ptr);
    int end_mem = MemoryManager::freeMemory();
    if ( end_mem <= start_mem) return false;
    return true;      
}

