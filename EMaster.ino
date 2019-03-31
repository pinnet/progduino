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


#include <avr/pgmspace.h>
#include <BasicTerm.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "Timer.h"
#include "EMaster.h"

BasicTerm term(&Serial);
Timer t; 
Sd2Card card;
SdVolume volume;
File root;

const String Version = "1.02A"; 

bool termMode = true;                         // Term Mode on off
const int chipSelect = 53;                    // SSD CE
byte g_cmd[80];                               // strings received from the controller will go in here
int dPage = 0;

static const int pageSize = 256;              // Size of Page
unsigned long  BAUD_RATE = 115200;            // Terminal Speed
byte pageBuff[pageSize];                      // Buffer for a page
byte currentPage = 0;                         // Current page of rom
int chunkSize;                                // Chunk Size of bulk mode transmission
byte curpos = 0;                              // Current position pointer
unsigned int romSize;                         // Size of rom to be decoded 
byte siunit;                                  // SIUnit used to display SD care sizes (K M Byte)
unsigned int filePointer;
int fileChunk = 16;
String cmd[3];
String filename = "default.rom";
String curdir[16];

SDStatus cardStatus  = SDS_UNMOUNTED;
EditorMode currentMode = MODE_BOOT; 
ViewMode view = VIEW_BOOT;
File myFile;


/*******************************************************************************************************************************
 *                                                                                                                         SETUP
 *  TODO: SD ERROR handling - check licence dependency { oscillate }
********************************************************************************************************************************/
void setup()
{
   SetHardwarePins(); 
   SetDataLinesAsInputs();
                                                                                                    // Load values from device ;
  siunit =    EEPROM.read(PSLOC_SIUNIT);
  chunkSize = EEPROM.read(PSLOC_CHUNKSIZE);
  romSize =   EEPROM.read(PSLOC_ROMSIZE) << 8;
  romSize +=  EEPROM.read(PSLOC_ROMSIZE + 1);
   
  Serial.begin(BAUD_RATE);
  memset(pageBuff,0,pageSize);                                                                            // Clear Page Buffer ;
  
  t.oscillate(PIN_LED_RED,100,LOW);
  
  SetAddress(0);
  //Serial.println( (MountSD() == SDS_MOUNTED)? "Mounted":" Unmounted");
  cardStatus = MountSD();
  DisplayCurrentPage();
  
}
/*******************************************************************************************************************************
 * 
 * TODO: Fix 'cancel input' bug                                                                                             LOOP
********************************************************************************************************************************/
void loop()
{
  static char buf[16];
  //memset(buf,0,16);
      
  if (readline(Serial.read(), buf, 16) > 0) {                                              // Read Serial until input end   ;
    char * p_args;                                                           
    p_args = strtok (buf," \n\r");                                                         // Split args into command array ;
    for(byte x=0; x < 3; x++){                                                             // loop around remaining input   ;
      cmd[x]= p_args;
      p_args = strtok (NULL, " \n\r");                                                     // split next input to command array ;
    }
    char key = cmd[0][0];                                                                  // get first char in string ;
    if (key < 97 && key > 65){                                                             // fix case of char to lower ;
      key += 32;
    }   

    if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f')){                          // 0-9 a-f for file chooser
        
        if (key == 'f'){                                                                    // f   Next list
            dPage++;
            DisplayCurrentPage();
        } else  if (key == '0'){                                                            // 0   Previous list 
            dPage--;
            if (dPage < 0) dPage = 0;
            DisplayCurrentPage();
        } else if (view == VIEW_DIRECTORY){
            char hex[2];
            hex[0] = '0';
            hex[1] = key;
            int filenum = strtol(hex, NULL, 16);
            filename = curdir[filenum];
            view = VIEW_FILE;
            DisplayCurrentPage();
        } else if (view == VIEW_INFO){
            char hex[2];
            hex[0] = '0';
            hex[1] = key;
            int filenum = strtol(hex, NULL, 16);
            Serial.println(curdir[filenum]);
        }
  } else if (key == 'g'){                                                                     // g 
        unsigned int arg = hexUint(cmd[1]);      
        currentPage = (arg & 0xFF00) >> 8;
        if((currentPage * 256) >= romSize){ currentPage = 0;}
        curpos = (arg & 0x00FF);
        if(termMode) DisplayCurrentPage(); 
      }
      else if (key == 'i'){                                                                   // i
        view = VIEW_INFO;
        DisplayCurrentPage();
      }
      else if (key == 'k'){                                                                   // k
        curpos += chunkSize; 
      }
      else if (key == 'l'){                                                                   // l
        
        if(cmd[1] !=0){ filename = cmd[1];
          view = VIEW_FILE;
        }
        else {  
          view = VIEW_DIRECTORY;
        }
        currentMode = MODE_FILE;
        DisplayCurrentPage();   
      }
      else if (key == "m"){                                                                   // m 
        cardStatus = MountSD();
      }
      else if (key == "n"){                                                                   // n 
        DisplayCurrentPage();
      }
      else if (key == 'o'){                                                                   // o  .
      
      } 
      else if (key == 'p'){                                                                   // p
          if (cmd[1].toInt() >= 0){
            currentPage = cmd[1].toInt(); 
            if((currentPage * 256) >= romSize){ currentPage = 0;}
            GetPage();
            DisplayCurrentPage();
        }
        else {
          DisplayCurrentPage(); 
        }  
      } 
      else if (key == 'r'){                                                                     // r  
        view = VIEW_INFO;
        if(termMode) DisplayCurrentPage();     
      }

      else if (key == 's'){                                                                     // s 
        
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

      else if (key == 't'){                                                                      // t 
        if (cmd[1] == "on"){
          termMode = true;
          SetTerm();
          DisplayCurrentPage(); 
          return;
        }
        else  if (cmd[1] == "off"){
          termMode = false; 
          return;    
        }        
        termMode = !termMode;
        if (termMode){
            SetTerm();
            DisplayCurrentPage(); 
        }
        else{
            UnsetTerm();
            DisplayCurrentPage();         
        }          
      }
      
      else if (key == "u"){                                                                     // u    
        if(cmd[1].length() != 1){      
          siunit = char('K');
          }  
        else {
          siunit = cmd[1][0];
          }    
        EEPROM.write(PSLOC_SIUNIT, siunit);
      }
      else if (key == 'v'){                                                                     // v
      verify();                 
      }   
      else if (key == 'w'){                                                                     // w


      if (currentMode == MODE_ROM){
          
          Serial.print("\r\nOutput to " + filename +". Saving.");

        if(cmd[1] != 0){ filename = cmd[1];}                               // todo. find a more robust filter 

        if (SD.exists(filename)){
            SD.remove(filename);
        } 
        myFile = SD.open(filename, FILE_WRITE);
        for (unsigned int a = 0 ; a <= (romSize / 256) ; a ++){
          currentPage = a ;
          GetRomPage(a);
          if(termMode) term.position(22,0);
          Serial.print("Writing page ");
          Serial.print(a);
          Serial.println(" to "+ filename);
          
          for (unsigned int ptr = 0; ptr < 256; ptr = ptr + fileChunk){
            
            SaveBuffer(ptr,fileChunk,myFile);
            
          } 
        //if(termMode) DisplayCurrentPage(); 
        }
        myFile.println();
        myFile.close(); 
        if(termMode) DisplayCurrentPage(); 
      }
      else if (currentMode == MODE_FILE)
      {
        unsigned int offset = 0;
        if(cmd[1] != 0) offset = cmd[1].toInt();

          // if (offset > (romSize / 256)) offset = 0;
              for (unsigned int a =0 ; a <= (romSize / 256) ; a ++){
                  currentPage = a ;
                  GetPage();
                  if(termMode) term.position(22,0);
                  Serial.print("Writing page ");
                  Serial.print(a + offset);
                  Serial.println(" to device");
                  WriteBufferToEEPROM((a + offset)* 256, 256);  
                }
      }
      
                    
      } 
      
      else if (key == 'x'){                                                               // x
        eeerase();
      
            
      }  
      else if (key == 'z'){                                                               // z  zero 
        FormatPage(0);
        if(termMode) term.position(22,0);
        Serial.print("Zeroing device ");
        WriteAllRom();
            
      } 
      else if (key == 'y'){                                                                // y

        if(cmd[1] != 0){ filename = cmd[1];}
        
        SD.remove(filename);
        filename = "default.rom";
      }  
      else if (key == '!'){                                                                 // !  
        currentPage = 0;
        curpos = 0; 
        if(termMode) DisplayCurrentPage();     
      }

      else if (key == ',' || key == '<' ){                                                  // < 
          currentPage --; 
        if((currentPage * 256) >= romSize){ currentPage = 0;}
        DisplayCurrentPage();   
      } 
      else if (key == '.' || key == '>' ){                                                   // >      
        
        currentPage ++;
        if((currentPage * 256) >= romSize ){ currentPage = 0;}
        DisplayCurrentPage();
        
      }
      else if (key == '@'){                                                                  // @  
        if (cmd[1].toInt() > 0){
          chunkSize = cmd[1].toInt();  
        } 
        else
        { 
          chunkSize = DEFAULT_CHUNKSIZE;
        }
        EEPROM.write(PSLOC_CHUNKSIZE, chunkSize);
      }
      else if (key == '?'|| key == 'h'){                                                     // ?
        HelpPage();
      }
      else if (key == '+'){                                                                  // +  
            if (curpos + chunkSize > 256){
              currentPage ++;
              if((currentPage * 256) >= romSize){ currentPage = 0;}
            }
            PrintBuffer(curpos,chunkSize,true);
      }
      else if (key == '%'){                                                                    // %
        FactoryReset();
      }
      else if (key == '#'){                                                                    // #

      // memset(pageBuff,0,pageSize);
        
          if (currentMode == MODE_ROM){
            currentMode = MODE_FILE;
            view = VIEW_FILE;
          }
          else if (currentMode == MODE_FILE){
            currentMode = MODE_ROM;
            view = VIEW_ROM;
          } 
          if (currentMode == MODE_BOOT){
            currentMode = MODE_ROM;
            view = VIEW_ROM;
          }
          DisplayCurrentPage(); 
      }
      else {                                                                                    // default
        if(termMode) DisplayCurrentPage();
        Serial.println(F("Command not recognised.\r\n")); 
        HelpPage(); 
      }
    if (!termMode) Serial.print("OK>_");
    }    
}
void GetPage(){
  
  if (currentMode == MODE_ROM){
        GetRomPage(currentPage);
     }
  else if (currentMode == MODE_FILE){
        GetFilePage(currentPage);
     }  
}
void DisplayCurrentPage(){
  GetPage();
  String line = "";
  
  if (termMode) SetTerm();
  if(view == VIEW_DIRECTORY){
    
  }
  if (currentMode == MODE_ROM){
       if (termMode)  term.set_color(R_headerForeground,R_headerBackground);
        line = "ROM :: Type 16c256 : Page " + String(currentPage) + " / "+ String((romSize /256)) +" : Size "+ String(romSize)+" bytes";
  }
  else if (currentMode == MODE_FILE){
       if (termMode)  term.set_color(F_headerForeground,F_headerBackground);
        line = "FILE :: " + filename + " : Page " + String(currentPage) + " / "+ String((romSize /256)) + " : "; 
        line += (cardStatus != SDS_MOUNTED) ?  "Not Mounted" : "file count";
   }
   else if (currentMode == MODE_BOOT){
     line = "? = help for more infomation";
    }

  if (termMode) term.cls();   
  if (line != ""){
  Serial.println("\t" + pad2center(" Eprom Master (c) Danny Arnold 2017 "+ Version + " ",56,"-"));                      //  header
  Serial.println("\t" + pad2center(line,56," "));
  Serial.println(F("\t-------------------------------------------------------- "));
  }
 //---------------------------------------------------------------------------------------------------------------------------------------------------------

if (view == VIEW_ROM){
  if (termMode) {
    term.set_color(romPageForeground,romPageBackground);
    blankpage();
  }
  displaybuff(pageBuff,0,pageSize);
}
else if (view == VIEW_DIRECTORY){
  if (termMode){
    term.set_color(romPageForeground,romPageBackground);
    blankpage();
  }
  myFile = SD.open("/");
  myFile.rewindDirectory();
  printDirectory(myFile,dPage,LIST);
  if (termMode)term.position(19,0);  
}
else if (view == VIEW_FILE){
  if (termMode){
    term.set_color(filePageForeground,filePageBackground);
     blankpage();
  }
  displaybuff(pageBuff,0,pageSize);    
}
else if (view == VIEW_INFO){
  if (termMode){
    term.set_color(romPageForeground,romPageBackground);
     blankpage();
     
  }
   myFile = SD.open("/DEVICE/");
   myFile.rewindDirectory();
   printDirectory(myFile,dPage,INFO);

   
}
else if (view == VIEW_BOOT){

    if (!SD.begin(24)) {
    Serial.println("initialization failed!");
    }
    else{
      if (!SD.exists(filename)){
        myFile = SD.open(filename, FILE_WRITE);
        myFile.close();
      }
    } 
    view = VIEW_ROM;
}
 //------------------------------------------------------------------------------------------------------------------------
  if (view == VIEW_ROM){
      if (termMode) term.set_color(R_footerForeground,R_footerBackground);
      line = "p (XXX) = page: > = next: < = back: s (XXK) = size";
  }
  else if (view == VIEW_FILE){
        if (termMode) term.set_color(F_footerForeground,F_footerBackground);
        line = "p (XXX) = page: > = next: < = back: s (XXK) = size";
  }
   else if (view == VIEW_DIRECTORY){
        if (termMode) term.set_color(F_footerForeground,F_footerBackground);
        line = "l = List Dir : Choose 1 - F : l (filename) = Load";
  }
  else if (view == VIEW_INFO){
       
        if (termMode){

          if(currentMode == MODE_FILE){
             term.set_color(F_footerForeground,F_footerBackground);}
          else{}

          
          term.position(19,0);
        }
        line = "Choose Device";
  }
  if (currentMode == MODE_BOOT){
        currentMode = MODE_ROM;
         
        line = "Press tab to continue";
       // return;
  }
  if (line != ""){
  Serial.println(F("\t-------------------------------------------------------- "));                       //   footer
  Serial.println("\t" + pad2center(line,58," "));
  Serial.println(F("\t-------------------------------------------------------- ")); 
  } 
}
void HelpPage(){
  char buffer[165];
  if (termMode){
    term.set_color(helpPageForeground,helpPageBackground);
    term.cls();
    term.position(0,0);
  }
  Serial.println("\r\nRom Master by Danny Arnold (2017) firmware version " + Version + "\r\n");
  Serial.println(F("-------------------------------------------------------------------------------- ")); 
  for (int i = 0; i < 16; i++){
    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); 
    Serial.print(buffer);           
  }
  Serial.println(F("Help\t\t\t?\tThis help page\r\n"));
}
static void displaybuff(byte *epm,word address,word datalength)
{
  char lbuf[82];
  char *x;
  int i,j;
  
  for (i=0; i < datalength; i+=16) {
    x=lbuf;
    sprintf(x,"   %02X%02X: ",currentPage,i);
    x+=9;
    for (j=0; j<16; j++) {
      sprintf(x," %02X",epm[i+j]);
      x+=3;
    }
    *x=32;
    x+=1;
    *x=32;
    x+=1;
    for (j=0; j<16; j++) {
      if (epm[i+j]>=32 && epm[i+j]<127) *x=epm[i+j];
      else *x=46;
      x++;
    }
    for (int space = 0; space < 5;space++){
        *x=32;
        x++;
    }
    *x=0;
    Serial.println(lbuf);   
  }
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
bool areyousure(){                      // TODO: make this work
  static char buf[2];
      
  if (readline(Serial.read(), buf, 2) > 0) {
    if (buf[0] == 'y') {
      return true;
    }
    else{  return false;   }
    }
 }

