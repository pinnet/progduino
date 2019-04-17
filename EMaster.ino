/*


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
#include <TimerThree.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <BasicTerm.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "EMaster.h"
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define TERM_HEIGHT 25
#define TERM_WIDTH  80
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1
#define PIN 5
#define NUMPIXELS 1
#define TIMEOUT 6000

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BasicTerm term(&Serial);
Sd2Card card;
SdVolume volume;
File root;

int16_t last, value;
const String Version = "1.04A"; 

bool termMode = true;                         // Term Mode on off
bool sure = false;                            // Sanity Check
const int chipSelect = 53;                    // SD CE
byte g_cmd[80];                               // strings received from the controller will go in here
int dPage = 0;

static const int pageSize = 256;              // Size of Page
unsigned long  BAUD_RATE = 115200;            // Terminal Speed
static byte pageBuff[pageSize];               // Buffer for a page
byte currentPage = 0;                         // Current page of rom
int chunkSize;                                // Chunk Size of bulk mode transmission
byte curpos = 0;                              // Current position pointer
unsigned int romSize;                         // Size of rom to be decoded 
byte siunit;                                  // SIUnit used to display SD care sizes (K M Byte)
unsigned int filePointer;
int fileChunk = 16;
String cmd[3];
String filename = "default.rom";
String lastfilename = filename;
String devicetype = "AT27C512";
String curdir[16];
static char buf[16];
unsigned int arg;
byte _encsw = 0;
byte _enca =  0;
byte _encb =  0;
unsigned int count = 0;

SDStatus cardStatus  = SDS_UNMOUNTED;
EditorMode currentMode = MODE_BOOT; 
ViewMode view = VIEW_BOOT;
File myFile;
SanityCheck doFunction = UNKNOWN;

/*******************************************************************************************************************************
 *                                                                                                                         SETUP
 *  TODO: SD ERROR handling - check licence dependency { oscillate }
********************************************************************************************************************************/
void setup()
{
    Timer3.initialize(10000);
    Timer3.attachInterrupt(timerint);

    pixels.begin(); 
    pixels.clear();
    pixels.show();
    display.setRotation(2);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
   
    term.init();
    SetHardwarePins(); 
    SetDataLinesAsInputs();
                                                                                                    // Load values from device ;
    siunit =    EEPROM.read(PSLOC_SIUNIT);
    chunkSize = EEPROM.read(PSLOC_CHUNKSIZE);
    romSize =   EEPROM.read(PSLOC_ROMSIZE) << 8;
    romSize +=  EEPROM.read(PSLOC_ROMSIZE + 1);

    Serial.begin(BAUD_RATE);
    memset(buf,0,16); 
    memset(pageBuff,0,pageSize);                                                                            // Clear Page Buffer ;
  
    //t.oscillate(PIN_LED_RED,100,LOW);
  
    SetAddress(0);
    Serial.println( (MountSD() == SDS_MOUNTED)? "Mounted":"Unmounted");
    cardStatus = MountSD();
    DisplayCurrentPage();
  
}
/*******************************************************************************************************************************
 * 
 *                                                                                                                          LOOP
********************************************************************************************************************************/

void loop()
{
  
  if ( readline(Serial.read(), buf, 16) > 0 ){                                              // Read Serial until input end   ;

    char key = parsecommand(buf);
    sure = false;
    if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f')){                          // 0-9 a-f for shortcut
        
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
        memset(buf,0,16);
  } else {
          switch (key){
            case 'g' :
                arg = hexUint(cmd[1]);      
                currentPage = (arg & 0xFF00) >> 8;
                if((currentPage * 256) >= romSize){ currentPage = 0;}
                curpos = (arg & 0x00FF);
                if(termMode) DisplayCurrentPage(); 
              break;
            case 'i' :
                view = VIEW_INFO;
                DisplayCurrentPage();
              break;
            case 'k' :
                curpos += chunkSize;
              break;
            case 'l' :
                loadfile();
              break;
            case 'm' :
                changeMode();
              break;
            case 'n' :
                filename = lastfilename;
                DisplayCurrentPage();
              break;
            case 'p' :
                setpage();
              break;
            case 'q' :
              break;
            case 'r' :
                setdevice();   
              break;
            case 's' :
                setsize();
              break;
            case 't' :
                terminalmode();         
              break;
            case 'u' :
                setunit();
              break;
            case 'v' :
                verify();  
              break;
            case 'w' :
                write();
              break;
            case 'x' :
                erase();
              break;
            case 'y' :
               sanitycheck();
              break;
            case 'z' :
                zero();
              break;
            case '!' :
                resetpage();
              break;
            case '<' :
                lastpage();
              break;
            case '>' :
                nextpage();
              break;
            case ',' :
                lastpage();
              break;
            case '.' :
                nextpage();
              break;
            case '@' :
                setchunksize();
              break;
            case '?' :
                HelpPage();
              break;
            case '+' :
                cont();
              break;
            case '%' :
                FactoryReset();
              break;
            case '#' :
                swappages();
              break;
            default :
              printinfo("Error","Command not recognised"); 
              HelpPage(); 
          }
          
          memset(buf,0,16);
    }
  }
  sleep();
}
int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  static bool escape = false;
  static int count = 0;
  int rpos;
if (termMode){
  term.show_cursor(false);
}

  if (readch > 0) {

    switch (readch) {

      case '$':
           if (termMode){
              buffer[pos++] = char('l');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;}
        break;
      case '\e':
           escape= true;
           buffer[pos] = 0;
           return;
        break;
      case '\f': // forward
       if (termMode){
          buffer[pos++] = char('.');
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
        }
        break;
      case '\b': // goback 
       if (termMode){
          buffer[pos--] = 0;
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
        }
        break;
        
      case '\t': // Return on TAB
        if (termMode){
          buffer[pos++] = char('#');
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
        }
        break; 
        
      case '\r': // Return on CR

        if (termMode && pos <=0){
            buffer[pos++] = char('.');
            rpos = pos;
            pos = 0;// Reset position index ready for next time
            return rpos;
        }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;

      case '=': // Return on CR

          if (termMode && pos <=0){
              buffer[pos++] = char('.');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;
          }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
      case '-': // Return on CR

          if (termMode && pos <=0){
              buffer[pos++] = char(',');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;
          }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
      default:
     
         if (pos < len-1) {
          
          if (escape){
            count += 1;
            if (count > 3){
              escape = false;
              count = 0;
              buffer[pos--] = 0;
              buffer[pos--] = 0;
              buffer[pos--] = 0;
            }
            
          }
          else{
            //Serial.print(char(readch));
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
          
        }
    }
    

    if (termMode) {
        term.position(22,0);
        Serial.print('['); 
        Serial.print(buffer);
        Serial.print("]                                 "); 
        term.position(22,0); 
     } 
  }

  // No end of line has been found, so return -1.
  return -1;
}
char parsecommand(char* buf){

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

  return char(key);
}

