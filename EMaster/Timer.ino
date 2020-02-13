/*
MIT License

Copyright (c) 2017 Danny Arnold

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
void pauseInt(bool state){

    if (state){
        encoderEnd();
        Timer3.detachInterrupt();  
    }
    else{
        encoderBegin();
        Timer3.attachInterrupt(timerint);
    }   

}
void sleep(){

    if(count <= TIMEOUT) return;
    count = 0;
    ResetDisplay = true;
}

void timerint(){

    count ++;
    if(!bufferLock){
        if (Serial.available() > 0){
            lineRdy = (readline(Serial.read(),buf,16) >=1) ? true : false;     
           
        }
    }
    if (count % 50) GetTime = true;
    if(timerRun){    
        if(count % 10 == 0 ) encswTimer ++;
        if(encswTimer >= LONG_PRESS) {
            ResetFunc();
        }
    }
    if(count % 15 == 0) pollLED();
    if(asyncRun){
        if(count % 100 == 0) asyncOLED(count/100);
    }

}

void timerStart(){

    encswTimer = 0;
    timerRun = true;

} 
unsigned int timerStop(){

    unsigned int retint = encswTimer;
    encswTimer = 0;
    timerRun = false;
    return retint;
}
