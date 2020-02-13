/*
//Copyright (c) 2017 Danny Arnold

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
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
#define NUMPIXELS 1
#define TIMEOUT 6000

// todo: ADAFRUIT need to remove dependancy
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// todo: atribute author in documentation and show licence
BasicTerm term(&Serial);

//Sd2Card card;
//SdVolume volume;
File root;

int16_t last, value;
const String Version = "1.07A"; 
bool termMode = true;                         // Term Mode on off
bool sure = false;                            // Sanity Check
const int chipSelect = 53;                    // SD CE
byte g_cmd[80];                               // strings received from the controller will go in here
int dPage = 0;
bool bufferLock = false;
bool lineRdy = true;
bool banners = true;

bool previousEncSW = false;
bool previousEncA = false;
bool previousEncB = false;

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
static char _buf[16];
unsigned int arg;
byte _encsw = 0;
byte _enca =  0;
byte _encb =  0;
unsigned int count = 0;
char defaultAction = '?';
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

    display.setRotation(2);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
  
    SetHardwarePins(); 
    SetDataLinesAsInputs();
   
    siunit =    EEPROM.read(PSLOC_SIUNIT);
    chunkSize = EEPROM.read(PSLOC_CHUNKSIZE);
    romSize =   EEPROM.read(PSLOC_ROMSIZE) << 8;
    romSize +=  EEPROM.read(PSLOC_ROMSIZE + 1);

    Serial.begin(BAUD_RATE);
    memset(buf,0,16); 
    memset(pageBuff,0,pageSize);                                                                           
    
    SetAddress(0);
    DisplayCurrentPage();
  
}
/*******************************************************************************************************************************
 * 
 *                                                                                                                          LOOP
********************************************************************************************************************************/

void loop()
{
    bufferLock = true;
    
    if(!buf[0] == 0){

        
        if(lineRdy){
            
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
                    case '\n' :
                    case '\r' :         
                      break;
                    case '^' :
                      //  uploadFile();
                        break;
                    case '~' :
                     //   downloadFile();
                        break;
                    case '?' :
                    case 'h' :
                        defaultAction = '?';
                        currentPage++;
                        HelpPage();
                        break;  
                    case 'i' :
                        defaultAction = '>';
                        view = VIEW_INFO;
                        DisplayCurrentPage();
                      break;
                    case 'k' :
                        curpos += chunkSize;
                      break;
                    case 'l' :
                        defaultAction = '>';
                        loadfile();
                      break;
                    case 'm' : 
                        defaultAction = '>';
                        changeMode();
                      break;
                    case 'n' :
                        defaultAction = 'p';
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
                        defaultAction = 'n';
                        write();
                      break;
                    case 'x' :
                        defaultAction = 'n';
                        erase();
                      break;
                    case 'y' :
                        sanitycheck();
                      break;
                    case 'z' :
                        defaultAction = 'n';
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
                    case '@' :
                        setchunksize();
                      break;
                    case '+' :
                        cont();
                      break;
                    case '%' :
                        FactoryReset();
                      break;
                    case '#' :
                        defaultAction = 'n';
                        SettingsPage();
                      break;
                    default :
                      if(termMode){
                        term.set_color(1,0);
                        serialPrintln("");
                        printinfo("Error","unrecognised command");
                        serialPrintln("  ? - for Help");
                      }
                  }
                  
                  memset(buf,0,16);
                
            }
        }
    }
    bufferLock = false;
    sleep();
}
