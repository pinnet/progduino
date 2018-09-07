#include <avr/pgmspace.h>
#include <BasicTerm.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "Timer.h"

#define PSLOC_SIUNIT 0
#define PSLOC_CHUNKSIZE 1
#define PSLOC_ROMSIZE 2
#define DEFAULT_SIUNIT char('M')
#define DEFAULT_ROMSIZE 0xFFFF
#define DEFAULT_CHUNKSIZE 16

#define PIN_A14 37
#define PIN_A12 38
#define PIN_A7  39
#define PIN_A6  40
#define PIN_A5  41
#define PIN_A4  42
#define PIN_A3  43
#define PIN_A2  44
#define PIN_A1  45
#define PIN_A0  46
#define PIN_D0  49
#define PIN_D1  48
#define PIN_D2  47

#define PIN_nWE 23
#define PIN_A13 25
#define PIN_A8  26
#define PIN_A9  27
#define PIN_A11 28
#define PIN_nOE 29
#define PIN_A10 30
#define PIN_nCE 31
#define PIN_D7  32
#define PIN_D6  33
#define PIN_D5  34
#define PIN_D4  35
#define PIN_D3  36

#define PIN_HEARTBEAT 13
#define PIN_LED_RED 13
#define PIN_LED_GREEN 12

const char hex[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

//To save ram NB use of PROGMEM to store long help screen to flash

const char string_0[]  PROGMEM = "Command\t\t\tInput\tExplanation\r\n\r\n"; 
const char string_1[]  PROGMEM = "Terminal Mode\tt (on/off)\tEnable/Disable Terminal Mode\r\n\t\t\t\toptional choices are on = Enable :\r\n\t\t\t\toff = Disable : (Default) toggle on/off\r\n" ;
const char string_2[]  PROGMEM = "Set Rom Size\t\ts (XXK)\tSet the size of the eprom\r\n\t\t\t\toptional choices are 1K : 2K : 4K :\r\n\t\t\t\t8K : 16K : 32K : (Default) 64K \r\n";
const char string_3[]  PROGMEM = "Page\t\t\tp (XXX)\tSet the current page \r\n\t\t\t\toptional choices are between 0 and the limit set\r\n\t\t\t\tby Rom Size : (Default) display current page.\r\n" ;
const char string_4[]  PROGMEM = "Directory\t\td (dir)\tSet current directory\r\n\t\t\t\toptional choices are fully quallified directory\r\n\t\t\t\tname : (Default) display current directory.\r\n";  
const char string_5[]  PROGMEM = "Next\t\t\tn\tNext page of rom.\r\n" ;
const char string_6[]  PROGMEM = "Back\t\t\tb\tBack to the previous page of rom.\r\n" ;
const char string_7[]  PROGMEM = "Reset\t\t\tx\tReset current page and address pointer to zero \r\n\t\t\t\t(address 0000)\r\n" ;
const char string_8[]  PROGMEM = "Mount SD Card\t\tm\tMount the SD card.\r\n";
const char string_9[]  PROGMEM = "Unit\t\t\tu (X)\tSet the SI unit for SD size reports\r\n\t\t\t\toptional choices are G = Gigabytes :\r\n\t\t\t\tM = Megabytes : K = Kilobyte : (Default) Bytes\r\n";
const char string_10[] PROGMEM = "Write\t\t\tw\tWrite to eprom TBC\r\n";
const char string_11[] PROGMEM = "Chunk Size (Bulk Mode)\tc (XXX)\tSet the Chunk Size of the data sent in Bulk Mode\r\n\t\t\t\toptional choices are number of bits :\r\n\t\t\t\t(Default) 16\r\n";
const char string_12[] PROGMEM = "Everything (Bulk Mode)\te\tOutput data in Chunk Size packets from \r\n\t\t\t\taddress 0000 to the size limit set by Rom Size\r\n";
const char string_13[] PROGMEM = "Address (Bulk Mode)\ta (XXXX)Set the current address\r\n\t\t\t\toptional choices a four bit hexidecimal number :\r\n\t\t\t\t(Default) 0000\r\n";
const char string_14[] PROGMEM = "Read (Bulk Mode)\tr\tOutput a Chunk Size of data from current address\r\n" ;
const char string_15[] PROGMEM = "acK (Bulk Mode)\t\tk\tHandshake for Read, will advance current address\r\n\t\t\t\tpointer by the size of Chunk Size\r\n" ;
const char* const string_table[] PROGMEM = {string_0, string_1, string_2, string_3, string_4, string_5,string_6, string_7, string_8, string_9, string_10, string_11,string_12, string_13, string_14, string_15};

enum EditorMode {
  MODE_ROM,
  MODE_FILE,
  MODE_BOOT
};
enum ViewMode {
  VIEW_DIRECTORY,
  VIEW_FILE,
  VIEW_ROM,
  VIEW_DEVICE,
  VIEW_BOOT,
  VIEW_INFO
};
enum SDStatus{
  SDS_MOUNTED,
  SDS_UNMOUNTED,
  SDS_VOLERROR
};
enum DirMode{
  LIST,
  INFO
  };




BasicTerm term(&Serial);
Timer t; 
Sd2Card card;
SdVolume volume;
File root;

const String Version = "1.01A"; 

bool termMode = true;            // Term Mode on off
const int chipSelect = 24;        // SSD CE
byte g_cmd[80];                   // strings received from the controller will go in here
int dPage = 0;

static const int pageSize = 256;  // Size of Page
unsigned long  BAUD_RATE = 115200;    // Terminal Speed
byte pageBuff[pageSize];          // Buffer for a page
byte currentPage = 0;             // Current page of rom
int chunkSize;                    // Chunk Size of bulk mode transmission
byte curpos = 0;                  // Current position pointer
unsigned int romSize;             // Size of rom to be decoded 
byte siunit;                      // SIUnit used to display SD care sizes (K M Byte)
unsigned int filePointer;
int fileChunk = 16;
String cmd[3];
String filename = "Default.rom";
String curdir[16];

SDStatus cardStatus  = SDS_UNMOUNTED;
EditorMode currentMode = MODE_BOOT; 
ViewMode view = VIEW_BOOT;

uint8_t R_headerBackground    = BT_BLUE;
uint8_t R_headerForeground    = BT_WHITE;
uint8_t R_footerBackground    = BT_BLUE;
uint8_t R_footerForeground    = BT_WHITE;
uint8_t F_headerBackground    = BT_WHITE;
uint8_t F_headerForeground    = BT_BLUE;
uint8_t F_footerBackground    = BT_WHITE;
uint8_t F_footerForeground    = BT_BLUE;


uint8_t romPageBackground   = BT_BLUE;
uint8_t romPageForeground   = BT_YELLOW;
uint8_t filePageBackground  = BT_WHITE;
uint8_t filePageForeground  = BT_BLACK;
uint8_t helpPageBackground  = BT_YELLOW;
uint8_t helpPageForeground  = BT_BLACK;
File myFile;
/*******************************************************************************************************************************
 *                                                                                                                         SETUP
********************************************************************************************************************************/
void setup()
{
   SetHardwarePins(); 
   SetDataLinesAsInputs();
  // Load values from EEPROM ;
  
  siunit =    EEPROM.read(PSLOC_SIUNIT);
  chunkSize = EEPROM.read(PSLOC_CHUNKSIZE);
  romSize =   EEPROM.read(PSLOC_ROMSIZE) << 8;
  romSize +=  EEPROM.read(PSLOC_ROMSIZE + 1);
   
  Serial.begin(BAUD_RATE);
  memset(pageBuff,0,pageSize);                      // Clear Page Buffer;
  
  t.oscillate(PIN_LED_RED,100,LOW);
  
  SetAddress(0);
  //Serial.println( (MountSD() == SDS_MOUNTED)? "Mounted":" Unmounted");
  cardStatus = MountSD();
  DisplayCurrentPage();
  
}
/*******************************************************************************************************************************
 *                                                                                                                         LOOP
********************************************************************************************************************************/
void loop()
{
  
  static char buf[16];
  //memset(buf,0,16);
      
  if (readline(Serial.read(), buf, 16) > 0) {
   
    char * pch;                                                            // Read Serial and split into commands and values
    pch = strtok (buf," \n\r");
    
    for(byte x=0; x < 3; x++)
    {
      cmd[x]= pch;
      pch = strtok (NULL, " \n\r");
    }
    char key = cmd[0][0];                                                   // get first char in string
    if (key < 97 && key > 65){                                             // lowercase only char 
      key += 32;
      }   
//Serial.println(key);
//    delay(500);   
    if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f')){        // 0-9 a-f for file chooser
      if (key == 'f'){
        
        dPage++;
        DisplayCurrentPage();
        } else  if (key == '0'){
        
        dPage--;
        if (dPage < 0) dPage = 0;
        DisplayCurrentPage();
        }
        
        else if (view == VIEW_DIRECTORY){
        char hex[2];
        hex[0] = '0';
        hex[1] = key;
        int filenum = strtol(hex, NULL, 16);
        filename = curdir[filenum];
        view = VIEW_FILE;
        DisplayCurrentPage();
      
      }
      else if (view == VIEW_INFO){
        char hex[2];
        hex[0] = '0';
        hex[1] = key;
        int filenum = strtol(hex, NULL, 16);
        Serial.println(curdir[filenum]);
        //view = VIEW_FILE;
        //DisplayCurrentPage();
      
      }
    }
    else if (key == 'g'){                                                 // g 
      unsigned int arg = hexUint(cmd[1]);      
      currentPage = (arg & 0xFF00) >> 8;
      if((currentPage * 256) >= romSize){ currentPage = 0;}
      curpos = (arg & 0x00FF);
      if(termMode) DisplayCurrentPage(); 
    }
    else if (key == 'i'){                                                 // i
      view = VIEW_INFO;
      DisplayCurrentPage();
    }
    else if (key == 'k'){                                                 // k
      curpos += chunkSize; 
    }
    else if (key == 'l'){                                                 // l
      
      if(cmd[1] !=0){ filename = cmd[1];
        view = VIEW_FILE;
      }
      else {  
        view = VIEW_DIRECTORY;
      }
      currentMode = MODE_FILE;
      DisplayCurrentPage();   
    }
    else if (key == "m"){                                                 // m 
      cardStatus = MountSD();
    }
    else if (key == "n"){                                                 // n 
      DisplayCurrentPage();
    }
    else if (key == 'o'){                                                 // o  .
     
     } 
    else if (key == 'p'){                                                 // p
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
    else if (key == 'r'){                                                 // r  
      view = VIEW_INFO;
      if(termMode) DisplayCurrentPage();     
    }

    else if (key == 's'){                                                 // s 
      
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

    else if (key == 't'){                                                 // t 
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
    
    else if (key == "u"){                                                 // u    
      if(cmd[1].length() != 1){      
        siunit = char('K');
        }  
      else {
        siunit = cmd[1][0];
        }    
      EEPROM.write(PSLOC_SIUNIT, siunit);
    }
    else if (key == 'v'){                                                 // v
    verify();                 
    }   
    else if (key == 'w'){                                                 // w


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
    
    else if (key == 'x'){                                                 // x
      eeerase();
     
          
    }  
    else if (key == 'z'){                                               // z  zero 
      FormatPage(0);
      if(termMode) term.position(22,0);
      Serial.print("Zeroing device ");
      WriteAllRom();
          
    } 
    else if (key == 'y'){                                                 // y

      if(cmd[1] != 0){ filename = cmd[1];}
      
      SD.remove(filename);
      filename = "default.rom";
    }  
    else if (key == '!'){                                                 // !  
      currentPage = 0;
      curpos = 0; 
      if(termMode) DisplayCurrentPage();     
    }

    else if (key == ',' || key == '<' ){                                  // < 
        currentPage --; 
       if((currentPage * 256) >= romSize){ currentPage = 0;}
       DisplayCurrentPage();   
    } 
    else if (key == '.' || key == '>' ){                                  // >      
      
      currentPage ++;
      if((currentPage * 256) >= romSize ){ currentPage = 0;}
      DisplayCurrentPage();
      
    }
    else if (key == '@'){                                                 // @  
      if (cmd[1].toInt() > 0){
        chunkSize = cmd[1].toInt();  
      } 
      else
      { 
        chunkSize = DEFAULT_CHUNKSIZE;
      }
       EEPROM.write(PSLOC_CHUNKSIZE, chunkSize);
    }
    else if (key == '?'|| key == 'h'){                                    // ?
      HelpPage();
    }
    else if (key == '+'){                                                 // +  
          if (curpos + chunkSize > 256){
            currentPage ++;
            if((currentPage * 256) >= romSize){ currentPage = 0;}
          }
           PrintBuffer(curpos,chunkSize,true);
    }
    else if (key == '%'){                                                 // %
      FactoryReset();
    }
    else if (key == '#'){                                                 // #

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
    else {                                                                    // default
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
  Serial.println("\t" + pad2center(" Eprom Master (c) Danny Arnold 2017 "+ Version + " ",56,"-"));        //  header
  Serial.println("\t" + pad2center(line,56," "));
  Serial.println(F("\t-------------------------------------------------------- "));
  }
 //-----------------------------------------------------------------------------------------------------------------------

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
   Serial.println("RESETING TO FACTORY DEFUALTS PLEASE WAIT"); 
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

