#include <ArduinoUnit.h>
#include "TaskManager.h"




void setup(){
    
    //Test::min_verbosity = TEST_VERBOSITY_ALL;
    Serial.begin(115200);
    Serial.println(F("---------------------------------"));
    Serial.println(F("Arduino Unit Test -   TaskManager"));
    Serial.println(F("---------------------------------"));
    while(!Serial) {}
    
}
void loop(){

    Test::run();
}