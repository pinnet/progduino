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

void WriteBufferToEEPROM(int addr, int size)
{
  
  digitalWrite(PIN_nOE, HIGH); // stop EEPROM from outputting byte
  digitalWrite(PIN_nWE, HIGH); // disables write
  SetDataLinesAsOutputs();
  delay(5);

  for (int x = 0; x < size; ++x)
  {
    SetAddress(addr + x);
    SetData(pageBuff[x]);
    delay(10);
    digitalWrite(PIN_nWE, LOW); // enable write
    delay(10);
    digitalWrite(PIN_nWE, HIGH); // disable write
   // t.update();
  }
  
 
}

void ReadIntoBuffer(unsigned int addr, int size)
{
  byte b=0;
  
  int x;
  for (x = 0; x < size; ++x)
  {
    SetAddress(addr + x);
    
    delayMicroseconds(100);
    b = ReadData();
    pageBuff[x] = b;
  }
  
}


void PrintBuffer( byte adr, int size,bool fileMode)
{

  if (!fileMode){
  Serial.print(hex[ (currentPage  & 0xF0) >> 4]);
  Serial.print(hex[ (currentPage & 0x0F) ]);
  Serial.print(hex[ (adr  & 0xF0) >> 4]);
  Serial.print(hex[ (adr & 0x0F) ]);
  Serial.print(":");
  }
  byte chk = 0;

  for (int x = adr; x <  size + adr; x++)
  {
   
    Serial.print(hex[ (pageBuff[x] & 0xF0) >> 4 ]);
    Serial.print(hex[ (pageBuff[x] & 0x0F)      ]);
   
    chk = chk ^ pageBuff[x];
  }
  if (!fileMode){
  Serial.print("=");
  Serial.print(hex[ (chk & 0xF0) >> 4 ]);
  Serial.print(hex[ (chk & 0x0F)      ]);
  Serial.println("");
  }
}

void GetRomPage(int page){

      int addr = 0;
      int x = 1;
      
      
      while (x < 5 && g_cmd[x] != 0)
      {
        addr = addr << 4;
        addr |= HexToVal(g_cmd[x++]);
      }     

      digitalWrite(PIN_nWE, HIGH); // disables write
      SetDataLinesAsInputs();
      digitalWrite(PIN_nOE, LOW); // makes the EEPROM output the byte
      delay(1);
      
      ReadIntoBuffer(addr + (page * pageSize), pageSize);
  
 }
 void FormatPage(char bitmap){

  for (int x = 0; x < 256;x++){ 
   pageBuff[x] = bitmap;
  }
  
  
  }
void WriteAllRom(){
      for (unsigned int a = 0 ; a <= (romSize / 256) ; a ++){
        currentPage = a ;      
        if(termMode) term.position(22,0);
        printinfo("Writing" ,"Page " + String(a)+" of "+ String(romSize / 256));
        WriteBufferToEEPROM(a * 256, 256);  
      }
}


