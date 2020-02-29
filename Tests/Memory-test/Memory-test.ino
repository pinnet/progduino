#include <ArduinoUnit.h>
#include "D:\dev\progduino\libraries\MemoryManager\MemoryManager.h"

MemoryManager MyMem();

test(this_should_work){

int x=3;
int y=4;
assertEqual(x,y);


}

void setup(){

    Serial.begin(115200);
    while(!Serial) {}


}
void loop(){

    Test::run();


}
