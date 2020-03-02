#include <ArduinoUnit.h>
#include "MemoryManager.h"

MemoryManager MyMem;
#define CALLOC_AMOUNT 7686 // 7712 Bytes Maximum alloaction MEGA2560 unknown bootloader

byte *pointer = NULL;
 
test(Test1A_FreeMemory){
    assertMore(MyMem.freeMemory(),0);
}
test(Test1B__calloc){

    int start_mem = MyMem.freeMemory();
    pointer = (byte *) calloc(CALLOC_AMOUNT , sizeof(byte));
    assertTrue((pointer != NULL));
    
    int end_mem = MyMem.freeMemory();
    assertLess(end_mem,start_mem);
}
test(Test1C__free){

    int start_mem = MyMem.freeMemory();
    free(pointer);
    int end_mem = MyMem.freeMemory();
    assertMore(end_mem,start_mem);

}
test(Test0_InitRAM){
    bool init = MyMem.initialiseRAM(CALLOC_AMOUNT);
    assertTrue(init);

}
void setup(){
    
    //Test::min_verbosity = TEST_VERBOSITY_ALL;
    Serial.begin(115200);
    Serial.println(F("---------------------------------"));
    Serial.println(F("Arduino Unit Test - MemoryManager"));
    Serial.println(F("---------------------------------"));
    while(!Serial) {}
    
}
void loop(){

    Test::run();
}
