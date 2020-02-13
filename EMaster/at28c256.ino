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

void eeerase(){
  
    digitalWrite(PIN_nOE, HIGH);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    SetAddress(0x5555);
    delayMicroseconds(50);
    SetData(0xAA);
    digitalWrite(PIN_nCE, LOW);
    digitalWrite(PIN_nWE, LOW); 
    delay(10);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    SetAddress(0x2AAA);
    delayMicroseconds(50);
    SetData(0x55);
    digitalWrite(PIN_nWE, LOW); 
    digitalWrite(PIN_nWE, LOW); 
    delay(10);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    SetAddress(0x5555);
    delayMicroseconds(50);
    SetData(0x80);
    digitalWrite(PIN_nWE, LOW); 
    digitalWrite(PIN_nWE, LOW); 
    delay(10);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    SetAddress(0x5555);
    delayMicroseconds(50);
    SetData(0xAA);
    digitalWrite(PIN_nWE, LOW); 
    digitalWrite(PIN_nWE, LOW); 
    delay(10);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    SetAddress(0x2AAA);
    delayMicroseconds(50);
    SetData(0x55);
    digitalWrite(PIN_nWE, LOW); 
    digitalWrite(PIN_nWE, LOW); 
    delay(10);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    SetAddress(0x5555);
    delayMicroseconds(50);
    SetData(0x10);
    digitalWrite(PIN_nWE, LOW); 
    digitalWrite(PIN_nWE, LOW); 
    delay(10);
    digitalWrite(PIN_nCE, HIGH);
    digitalWrite(PIN_nWE, HIGH); 
    delay(10);
    delay(200);
  }
  

