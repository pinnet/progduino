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
#define DEFAULT_DEBOUNCE 100
#define MAX_BRIGHT 30
#define EASE_TIME 500
#define FLASH_TIME 100
#define LONG_PRESS 25


#define ROM_OFFSET 8    
#define FILE_OFFSET 0;    
#define SETTING_OFFSET 14;    


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
#define PIN_D0  47
#define PIN_D1  48
#define PIN_D2  49

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

#define ENCSW  2
#define ENCA   3
#define ENCB   4

#define LED_PIN 11
#define LED_SER 12

const char hex[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};
enum EncoderSW {
  UP,DOWN
};
enum EncoderDIR {
  FORWARD,BACKWARD
};
enum EditorMode {
  MODE_ROM,
  MODE_FILE,
  MODE_BOOT
};

enum MenuItem {
  _FILE_MENUPAGE = 0,
  _FILE_LOAD = 1,
  _FILE_NEW = 2,
  _FILE_WRITE = 3,
  _FILE_VERIFY = 4,
  _FILE_ERASE = 5,
  _FILE_RENAME = 6,
  _FILE_EXIT = 7,
  _ROM_MENUPAGE = 8,
  _ROM_CHANGE = 9,
  _ROM_WRITE = 10,
  _ROM_ERASE = 11,
  _ROM_VERIFY = 12,
  _ROM_EXIT = 13,
  _SETTING_MENUPAGE = 14,
  _SETTING_DAY_TIME =15,
  _SETTING_SER_BAUD =16,
  _SETTING_USB_BAUD =17,
  _SETTING_CHUNKSIZE =18,
  _SETTING_SLEEP_TIMEOUT =19,
  _SETTING_EXIT =20
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
enum LEDState{
  OFF,THROB,FLASH,STEADY
};
enum LEDColour{
  RED,GREEN,BLUE,ORANGE,YELLOW,PURPLE,CYAN
};
enum SanityCheck{
  UNKNOWN,WRITE,ERASE,ZERO
};
enum FileType{
  TXT,ROM,DEV,MD5,INI

};

uint8_t R_headerBackground    = BT_BLACK;
uint8_t R_headerForeground    = BT_WHITE;
uint8_t R_footerBackground    = BT_BLACK;
uint8_t R_footerForeground    = BT_WHITE;
uint8_t F_headerBackground    = BT_BLACK;
uint8_t F_headerForeground    = BT_WHITE;
uint8_t F_footerBackground    = BT_BLACK;
uint8_t F_footerForeground    = BT_WHITE;
uint8_t cmdlineForeground     = BT_CYAN;
uint8_t cmdlineBackground    = BT_BLUE;
uint8_t romPageBackground   = BT_BLACK;
uint8_t romPageForeground   = BT_GREEN;
uint8_t filePageBackground  = BT_BLACK;
uint8_t filePageForeground  = BT_BLUE;
uint8_t helpPageBackground  = BT_BLACK;
uint8_t helpPageForeground  = BT_WHITE;
uint8_t settingsPageBackground  = BT_BLACK;
uint8_t settingsPageForeground  = BT_BLUE;
uint8_t alertPageForeground  = BT_YELLOW;
uint8_t alertPageBackground  = BT_BLUE;
uint8_t errorPageForeground  = BT_WHITE;
uint8_t errorPageBackground  = BT_RED;