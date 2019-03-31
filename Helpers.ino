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
  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);
  pinMode(PIN_D2, INPUT);
  pinMode(PIN_D3, INPUT);
  pinMode(PIN_D4, INPUT);
  pinMode(PIN_D5, INPUT);
  pinMode(PIN_D6, INPUT);
  pinMode(PIN_D7, INPUT);
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

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  static bool escape = false;
  static int count = 0;
  int rpos;

  if (readch > 0) {

    
    
    switch (readch) {

      case '\e':
       escape= true;
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
     
      default:
     
         if (pos < len-1) {
          
          if (escape){
            escape = true;
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
            Serial.print(char(readch));
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
          
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}
void blankpage(){

  term.position(3,0);
  for (byte ln=0;ln < 16;ln ++){
    
    for (int space =0;space < 80;space ++){
      Serial.print(' ');      
    }
    Serial.println();
    
  }
  term.position(3,0);
}


