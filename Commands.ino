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

void SetHardwarePins(){ 

  //pinMode(chipSelect,  OUTPUT);
  //digitalWrite(chipSelect, HIGH );

  pinMode(ENCSW,  INPUT_PULLUP); 
  pinMode(ENCA,   INPUT_PULLUP);
  pinMode(ENCB,   INPUT_PULLUP);
  pinMode(PIN_A0,  OUTPUT);
  pinMode(PIN_A1,  OUTPUT);
  pinMode(PIN_A2,  OUTPUT);
  pinMode(PIN_A3,  OUTPUT);
  pinMode(PIN_A4,  OUTPUT);
  pinMode(PIN_A5,  OUTPUT);
  pinMode(PIN_A6,  OUTPUT);
  pinMode(PIN_A7,  OUTPUT);
  pinMode(PIN_A8,  OUTPUT);
  pinMode(PIN_A9,  OUTPUT);
  pinMode(PIN_A10, OUTPUT);
  pinMode(PIN_A11, OUTPUT);
  pinMode(PIN_A12, OUTPUT);
  pinMode(PIN_A13, OUTPUT);
  pinMode(PIN_A14, OUTPUT);
  pinMode(PIN_nCE, OUTPUT); digitalWrite(PIN_nCE, LOW); 
  pinMode(PIN_nOE, OUTPUT); digitalWrite(PIN_nOE, HIGH);
  pinMode(PIN_nWE, OUTPUT); digitalWrite(PIN_nWE, HIGH);
  pinMode(PIN_HEARTBEAT, OUTPUT);
  digitalWrite(PIN_HEARTBEAT, HIGH);
  
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_RED, LOW);
  
  pinMode(PIN_LED_GREEN, OUTPUT);
  digitalWrite(PIN_LED_GREEN, LOW);

  }
  void SetDataLinesAsInputs()
{
  pinMode(PIN_D0, INPUT_PULLUP);
  pinMode(PIN_D1, INPUT_PULLUP);
  pinMode(PIN_D2, INPUT_PULLUP);
  pinMode(PIN_D3, INPUT_PULLUP);
  pinMode(PIN_D4, INPUT_PULLUP);
  pinMode(PIN_D5, INPUT_PULLUP);
  pinMode(PIN_D6, INPUT_PULLUP);
  pinMode(PIN_D7, INPUT_PULLUP);
}

void SetDataLinesAsOutputs()
{
  pinMode(PIN_D0, OUTPUT);
  pinMode(PIN_D1, OUTPUT);
  pinMode(PIN_D2, OUTPUT);
  pinMode(PIN_D3, OUTPUT);
  pinMode(PIN_D4, OUTPUT);
  pinMode(PIN_D5, OUTPUT);
  pinMode(PIN_D6, OUTPUT);
  pinMode(PIN_D7, OUTPUT);
}

void SetAddress(int a)
{
  digitalWrite(PIN_A0,  (a&1)?HIGH:LOW    );
  digitalWrite(PIN_A1,  (a&2)?HIGH:LOW    );
  digitalWrite(PIN_A2,  (a&4)?HIGH:LOW    );
  digitalWrite(PIN_A3,  (a&8)?HIGH:LOW    );
  digitalWrite(PIN_A4,  (a&16)?HIGH:LOW   );
  digitalWrite(PIN_A5,  (a&32)?HIGH:LOW   );
  digitalWrite(PIN_A6,  (a&64)?HIGH:LOW   );
  digitalWrite(PIN_A7,  (a&128)?HIGH:LOW  );
  digitalWrite(PIN_A8,  (a&256)?HIGH:LOW  );
  digitalWrite(PIN_A9,  (a&512)?HIGH:LOW  );
  digitalWrite(PIN_A10, (a&1024)?HIGH:LOW );
  digitalWrite(PIN_A11, (a&2048)?HIGH:LOW );
  digitalWrite(PIN_A12, (a&4096)?HIGH:LOW );
  digitalWrite(PIN_A13, (a&8192)?HIGH:LOW );
  digitalWrite(PIN_A14, (a&16384)?HIGH:LOW);
}

void SetData(byte b)
{
  digitalWrite(PIN_D0, (b&1)?HIGH:LOW  );
  digitalWrite(PIN_D1, (b&2)?HIGH:LOW  );
  digitalWrite(PIN_D2, (b&4)?HIGH:LOW  );
  digitalWrite(PIN_D3, (b&8)?HIGH:LOW  );
  digitalWrite(PIN_D4, (b&16)?HIGH:LOW );
  digitalWrite(PIN_D5, (b&32)?HIGH:LOW );
  digitalWrite(PIN_D6, (b&64)?HIGH:LOW );
  digitalWrite(PIN_D7, (b&128)?HIGH:LOW);
}

byte ReadData()
{
  byte b = 0;

  if (digitalRead(PIN_D0) == HIGH) b |= 1;
  if (digitalRead(PIN_D1) == HIGH) b |= 2;
  if (digitalRead(PIN_D2) == HIGH) b |= 4;
  if (digitalRead(PIN_D3) == HIGH) b |= 8;
  if (digitalRead(PIN_D4) == HIGH) b |= 16;
  if (digitalRead(PIN_D5) == HIGH) b |= 32;
  if (digitalRead(PIN_D6) == HIGH) b |= 64;
  if (digitalRead(PIN_D7) == HIGH) b |= 128;

  return(b);
}

// converts one character of a HEX value into its absolute value (nibble)
byte HexToVal(byte b)
{
  if (b >= '0' && b <= '9') return(b - '0');
  if (b >= 'A' && b <= 'F') return((b - 'A') + 10);
  if (b >= 'a' && b <= 'f') return((b - 'a') + 10);
  return(0);
}
String pad2center(String str,int size,String padding){
  String pad = "";
  int  n = str.length();
  if (n >= size) return str;
  
  for (int a = 1; a <= (size - n )/ 2; a++){
     pad += padding;
  }
  return pad + str + pad;
}
String padFromDividor(String str,int size,String padding,String dividor){
  String pad = "";
  int  n = str.length();
  if (n >= size) return str;
  
  for (int a = 1; a <= (size - n )/ 2; a++){
     pad += padding;
  }
  int cutline = str.indexOf(dividor);
  return str.substring(0,cutline) + pad + pad + str.substring(cutline,str.length());
}
void SetTerm(){
        term.init();
        term.set_attribute(BT_BOLD);
        term.cls();
        term.position(0,0);
        term.show_cursor(true);
}
void UnsetTerm(){
  
      term.set_attribute(BT_NORMAL);
      term.set_color(BT_WHITE,BT_BLACK);
      term.cls();
      term.position(0,0);  
}
unsigned int hexUint(String hex){
  
  char crs[6];
  hex.toCharArray(crs,6);
  return strtol (crs,NULL,16);
}


void oledOut(String head, String msg){
    display.setCursor(0, 0);
    display.clearDisplay();
    display.setTextSize((head.length() < 10)? 2 : 1);
    display.setTextColor(WHITE);             
    display.println(pad2center(head,(head.length() < 10)? 10 : 20," "));
     
    display.setTextSize(1); 
    display.println();       
    display.println(pad2center(msg,20," "));
    display.display();
    //display.startscrollright(0x00, 0x0F);
}
void changeMode(){

      if(cmd[1] != 0){ 
          
        if(cmd[1][0]== 'f' || cmd[1][0] == 'F' ){
            currentMode = MODE_FILE;
            view = VIEW_FILE;

        } else if(cmd[1][0]== 'd' || cmd[1][0] == "D" ){

            currentMode = MODE_ROM;
            view = VIEW_ROM;
        }
        DisplayCurrentPage(); 
      } else {
        swappages();
      }
}
void zero(){

  doFunction=ZERO;
  if (!sure && termMode){
     printinfo("Zero device ","Are you Sure ?");
  } else {

      FormatPage(0);
      if(termMode) term.position(22,0);
      
      WriteAllRom();
      sure = false;
      doFunction= UNKNOWN;
      memset(buf,0,16);
      DisplayCurrentPage();
  }
}
bool verifyPage(byte page){

    byte b = 0;
    int addr = page * pageSize;
   
    GetRomPage(page);
    myFile=SD.open(filename);
    
    for ( int i= 0;i < pageSize;i++){
    
      myFile.seek(addr + i);
      b = myFile.read();
      //printinfo("Verify "+String(addr +i) +":"+String(b),String(i) +":"+ String(pageBuff[i]));
      if (pageBuff[i] != b){
        return false;
      }
      
  }
  myFile.close();
  return true;
}
void verify(){
  unsigned int addr = 0;
  static int size = pageSize;
  byte b = 0;
  

  myFile=SD.open(filename);
    for (unsigned int a = 0 ; a <= (romSize / 256) ; a ++){
      currentPage = a ; 
      GetRomPage(a);
      if(termMode) term.position(22,0);
      printinfo("Verify Device Page "+String(a),filename);
      
      for ( int i= 0;i < size;i++){

        myFile.seek(addr); 
        b = myFile.read();
     if (pageBuff[i] != b){
      printinfo("Error","Device verify FAIL");
      Serial.print(a,DEC);
      Serial.print("-");
      Serial.print(i,HEX);
      Serial.print(":");
      Serial.print(pageBuff[i],HEX);
      Serial.print("!=");
      Serial.println(b,HEX);
        
      myFile.close();
      return;
     }
     addr ++;
    }
  }
  myFile.close();
  if(termMode) term.position(22,0);
  printinfo("Verify","Device Pass");
}
void setchunksize(){
  if (cmd[1].toInt() > 0){
          chunkSize = cmd[1].toInt();  
        } 
        else
        { 
          chunkSize = DEFAULT_CHUNKSIZE;
        }
        EEPROM.write(PSLOC_CHUNKSIZE, chunkSize);

}

void loadfile(){
  if(cmd[1] !=0){ 
    
    filename = cmd[1];
    view = VIEW_FILE;
  } else {  
    view = VIEW_DIRECTORY;
  }
  currentMode = MODE_FILE;
  DisplayCurrentPage();  
}

void setdevice(){
    view = VIEW_INFO;
    if(termMode) DisplayCurrentPage();  
}
void setsize(){
      romSize = 0xFFFF;
      if      (cmd[1] == "1K"  | cmd[1] == "1k" ){ romSize = romSize / 64;}
      else if (cmd[1] == "2K"  | cmd[1] == "2k" ){ romSize = romSize / 32;}
      else if (cmd[1] == "4K"  | cmd[1] == "4k" ){ romSize = romSize / 16;}
      else if (cmd[1] == "8K"  | cmd[1] == "8k" ){ romSize = romSize / 8;}
      else if (cmd[1] == "16K" | cmd[1] == "16k"){ romSize = romSize / 4;}
      else if (cmd[1] == "32K" | cmd[1] == "32k"){ romSize = romSize / 2;}
      EEPROM.write(PSLOC_ROMSIZE,(romSize & 0xFF00) >> 8);
      EEPROM.write(PSLOC_ROMSIZE + 1, romSize & 0x00FF);
      if(termMode) DisplayCurrentPage();

}
void terminalmode(){
  if (cmd[1] == "on"){
      termMode = true;
      SetTerm();
      DisplayCurrentPage(); 
      return;
  } else  if (cmd[1] == "off"){
      termMode = false;  
      return; }        
  termMode = !termMode;
  if (termMode){
      SetTerm();
      DisplayCurrentPage(); 
  } else {
      UnsetTerm();
      DisplayCurrentPage();} 

}
void setunit(){
      if(cmd[1].length() != 1){      
         siunit = char('K');}  
      else { siunit = cmd[1][0]; }    
         EEPROM.write(PSLOC_SIUNIT, siunit);
}
void erase(){


  doFunction=ERASE;
  lastfilename = filename;
  if (view == VIEW_DIRECTORY){

    if (cmd[1]){

        if (sizeof(cmd[1]) > 1){
          
          filename = cmd[1];
        }else if (sizeof(cmd[1]) == 1){
            char hex[2];
            hex[0] = '0';
            hex[1] = cmd[1][0];
            int filenum = strtol(hex, NULL, 16);
            filename = curdir[filenum];
        } 
    } 


    if (!sure && termMode){
            printinfo("Erase "+ filename,"Are you Sure ?");
        } else {
            printinfo("Erasing",filename);
            SD.remove(filename);
            filename = "default.rom";
            DisplayCurrentPage();
        }   
  } else if (currentMode == MODE_ROM){
      if (!sure && termMode){
        printinfo("Erase Device","Are you Sure ?");
      } else {  
            eeerase(); 
      }
      
  }
}
void sanitycheck(){
  sure =  true;
  switch(doFunction){

      case WRITE :
        write();
        break;
      case ERASE :
        erase();
        break;
      case ZERO :
        zero();
        break;
      default :
        break;
  }


}
void write(){

  if(cmd[1]){ filename = cmd[1];} 

  doFunction=WRITE;
  
  if (currentMode == MODE_ROM){
     if (!sure && termMode){
        printinfo("Write to "+ filename,"Are you Sure ?");
     } else { 
           // todo. find a more robust filter 

        if (SD.exists(filename)){
            SD.remove(filename);
        } 
        myFile = SD.open(filename, FILE_WRITE);
        for (unsigned int a = 0 ; a <= (romSize / 256) ; a ++){
          currentPage = a ;
          GetRomPage(a);
          if(termMode) term.position(22,0);
          printinfo("Writing page "+ String(a),"to "+ filename);
          for (unsigned int ptr = 0; ptr < 256; ptr = ptr + fileChunk){
            
            SaveBuffer(ptr,fileChunk,myFile);
            
          } 
        //if(termMode) DisplayCurrentPage(); 
        }
        myFile.println();
        myFile.close();
        currentPage = 0; 
        if(termMode) DisplayCurrentPage(); 
      }
    } else if (currentMode == MODE_FILE){
      unsigned int offset = 0;
      if(cmd[1]) offset = cmd[1].toInt();
      if (!sure && termMode){
            printinfo("Write to device","Are you Sure ?");
      } else { 
             // if (offset > (romSize / 256)) offset = 0;
            for (unsigned int a =0 ; a <= (romSize / 256) ; a ++){
              myFile = SD.open(filename);
              currentPage = a ;
              GetFilePage(a);
              if(termMode) term.position(22,0);
              printinfo("Writing page "+ String(a + offset),"from "+ filename);
              WriteBufferToEEPROM((a + offset)* 256, 256);
              myFile.close();
              delay(100); 
              if(!verifyPage(a)){
                  printinfo("Error","Writing to Device");
                  return;
              } 
            }
            currentPage = 0; 
            if(termMode) DisplayCurrentPage(); 
      }
    }
      sure = false;
      memset(buf,0,16);     
  }

void(* ResetFunc) (void) = 0; //declare reset function @ address 0
void FactoryReset(){
   if (termMode){  term.set_attribute(BT_BLINK); }
   Serial.println("RESETING TO FACTORY DEFAULTS PLEASE WAIT"); 
   EEPROM.write(PSLOC_SIUNIT, DEFAULT_SIUNIT);
   EEPROM.write(PSLOC_CHUNKSIZE, DEFAULT_CHUNKSIZE);
   EEPROM.write(PSLOC_ROMSIZE, (DEFAULT_ROMSIZE & 0xFF00) >> 8);
   EEPROM.write(PSLOC_ROMSIZE + 1, (DEFAULT_ROMSIZE & 0x00FF) );
   
   delay(1500);
   if (termMode){ UnsetTerm(); }
   delay(1500);
   ResetFunc();
}
bool areyousure(){

  static char buf[2];
      
  if (readline(Serial.read(), buf, 2) > 0) {
    if (buf[0] == 'y') {
      return true;
    }
    else{  return false;   }
    }
 }
