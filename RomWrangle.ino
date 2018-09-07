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
    t.update();
  }
  
  digitalWrite(PIN_HEARTBEAT, LOW);
  digitalWrite(PIN_LED_RED, LOW);
 
}

void ReadIntoBuffer(unsigned int addr, int size)
{
  byte b=0;
  digitalWrite(PIN_LED_GREEN, HIGH);
  
  int x;
  for (x = 0; x < size; ++x)
  {
    SetAddress(addr + x);
    
    delayMicroseconds(100);
    b = ReadData();
    pageBuff[x] = b;
  }
  
  digitalWrite(PIN_LED_GREEN, LOW);
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
        if(termMode) term.position(23,0);
        Serial.print("Page ");
        Serial.print(a);
        Serial.print(" of ");
        Serial.print((romSize / 256));
        WriteBufferToEEPROM(a * 256, 256);  
      }
}


