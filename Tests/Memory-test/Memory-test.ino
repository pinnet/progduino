#include <ArduinoUnit.h>
#include "MemoryManager.h"

MemoryManager MyMem;
#define CALLOC_AMOUNT 0x1E20 // 7712 Bytes Maximum alloaction MEGA2560 unknown bootloader

uint16_t pointer = NULL;
 
test(Test1_FreeMemory){
    assertMore(MyMem.freeMemory(),0);
}
test(Test2_calloc){

    uint16_t start_mem = MyMem.freeMemory();
    pointer = (byte *) calloc(CALLOC_AMOUNT , sizeof(byte));
    assertNotEqual(pointer,NULL);
    uint16_t end_mem = MyMem.freeMemory();
    assertLess(end_mem,start_mem);

}
test(Test3_free){

    uint16_t start_mem = MyMem.freeMemory();
    free(pointer);
    assertNotEqual(pointer,NULL);
    uint16_t end_mem = MyMem.freeMemory();
    assertMore(end_mem,start_mem);

}
void setup(){
    Test::min_verbosity = TEST_VERBOSITY_ALL;
    Serial.begin(115200);
    Serial.println(F("---------------------------------"));
    Serial.println(F("Arduino Unit Test - MemoryManager"));
    Serial.println(F("---------------------------------"));
    while(!Serial) {}
    
}
void loop(){

    Test::run();
}
