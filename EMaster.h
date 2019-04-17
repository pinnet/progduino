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

#define ENCSW  13
#define ENCA    7
#define ENCB    6

#define PIN_HEARTBEAT 4
#define PIN_LED_RED 5
#define PIN_LED_GREEN 4

const char hex[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

//To save ram NB use of PROGMEM to store long help screen to flash

const char string_0[]  PROGMEM = "Command\t\t\tInput\tExplanation\r\n\r\n"; 
const char string_1[]  PROGMEM = "Terminal Mode\tt (on/off)\tEnable/Disable Terminal Mode\r\n\t\t\t\toptional choices are on = Enable :\r\n\t\t\t\toff = Disable : (Default) toggle on/off\r\n" ;
const char string_2[]  PROGMEM = "Set device size\t\ts (XXK)\tSet the size of the device\r\n\t\t\t\toptional choices are 1K : 2K : 4K :\r\n\t\t\t\t8K : 16K : 32K : (Default) 64K \r\n";
const char string_3[]  PROGMEM = "Page\t\t\tp (XXX)\tSet the current page \r\n\t\t\t\toptional choices are between 0 and the limit set\r\n\t\t\t\tby Rom Size : (Default) display current page.\r\n" ;
const char string_4[]  PROGMEM = "Directory\t\td (dir)\tSet current directory\r\n\t\t\t\toptional choices are the full path of directory\r\n\t\t\t\tname : (Default) display current directory.\r\n";  
const char string_5[]  PROGMEM = "Next\t\t\tn\tNext page of device.\r\n" ;
const char string_6[]  PROGMEM = "Back\t\t\tb\tBack to the previous page of device.\r\n" ;
const char string_7[]  PROGMEM = "^Reset\t\t\tx\tReset current page and address pointer to zero \r\n\t\t\t\t(address 0000)\r\n" ;
const char string_8[]  PROGMEM = "Write\t\t\tw\tWrite to device TBC\r\n";
const char string_9[]  PROGMEM = "Unit\t\t\tu (X)\tSet the \r\n\t\t\t\toptional choices are G = Gigabytes :\r\n\t\t\t\tM = Megabytes : K = Kilobyte : (Default) Bytes\r\n";
const char string_10[] PROGMEM = "";
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
enum EncoderState{
  FORWARD,
  BACKWARD,
  SELECTED,
  A,
  B
  };
enum SanityCheck{
  UNKNOWN,WRITE,ERASE,ZERO
};
  
uint8_t R_headerBackground    = BT_BLACK;
uint8_t R_headerForeground    = BT_YELLOW;
uint8_t R_footerBackground    = BT_BLACK;
uint8_t R_footerForeground    = BT_YELLOW;
uint8_t F_headerBackground    = BT_BLACK;
uint8_t F_headerForeground    = BT_WHITE;
uint8_t F_footerBackground    = BT_BLACK;
uint8_t F_footerForeground    = BT_WHITE;


uint8_t romPageBackground   = BT_BLACK;
uint8_t romPageForeground   = BT_GREEN;
uint8_t filePageBackground  = BT_BLACK;
uint8_t filePageForeground  = BT_YELLOW;
uint8_t helpPageBackground  = BT_BLACK;
uint8_t helpPageForeground  = BT_GREEN;
