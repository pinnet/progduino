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
  pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN,HIGH);
  pinMode(LED_SER, OUTPUT); digitalWrite(LED_SER,HIGH);

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

void pollLED(){
    
    int _col = 0;
    switch(led_state){

      case FLASH :
        if (flash){ 
           _col = (int)flashing.SetSetpoint(MAX_BRIGHT);
          if ( _col >= MAX_BRIGHT) flash = false;
        }
       else { 
          _col = (int)flashing.SetSetpoint(0);
          if ( _col <= 0) flash = true;
        }
      break;


      case THROB :
        if (easein){ 
           _col = (int)easing.SetSetpoint(MAX_BRIGHT);
          if ( _col >= MAX_BRIGHT) easein = false;
        }
       else { 
          _col = (int)easing.SetSetpoint(0);
          if ( _col <= 0) easein = true;
        }
      break;
 
      case STEADY :
      _col = MAX_BRIGHT;
      break;

      case OFF :
        _col = 0;
      break;
    }
  
    switch (led_colour){

      case RED :
        pixels.setPixelColor(0,_col,0,0);
      break;
      case GREEN :
        pixels.setPixelColor(0,0,_col,0);
      break;
      case BLUE :
        pixels.setPixelColor(0,0,0,_col);
      break;
      case YELLOW :
        pixels.setPixelColor(0,_col,_col,0);
      break;
      case ORANGE :
        pixels.setPixelColor(0,_col,_col * 0.75,0);
      break;
      case PURPLE :
        pixels.setPixelColor(0,_col * 0.50,0,_col);
      break;
      case CYAN :
        pixels.setPixelColor(0,0,_col,_col);
      break;
    }
    
    pixels.show();
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


unsigned int hexUint(String hex){
  
  char crs[6];
  hex.toCharArray(crs,6);
  return strtol (crs,NULL,16);
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
bool areyousure(){

  static char buf[2];
      
  if (readline(Serial.read(), buf, 2) > 0) {
    if (buf[0] == 'y') {
      return true;
    }
    else{  return false;   }
    }
 }

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  static bool escape = false;
  static int count = 0;
  int rpos;

  if (readch > 0) {

    switch (readch) {

      case '/':
           if (termMode){
              buffer[pos++] = char('l');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;}
        break;
      case '\e':
              defaultAction = 'p';
              buffer[pos++] = char('p');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;
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
          buffer[pos] = 0;
          rpos = pos--;
          pos = 0;// Reset position index ready for next time
          return rpos;
        }
        break;
        
      case '\t': // Return on TAB
        if (termMode){
          command_line = ">";
          buffer[pos++] = char('m');
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
        }
        break; 
        
      case '\r': // Return on CR

        if (termMode && pos <=0){
            buffer[pos++] = char(defaultAction);
            rpos = pos;
            pos = 0;// Reset position index ready for next time
            return rpos;
        }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
      case '?': // Return on CR

          if (termMode && pos <=0){
              buffer[pos++] = char('?');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;
          }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
      case '=': // Return on CR

          if (termMode && pos <=0){
              buffer[pos++] = char('>');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;
          }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
      case '-': // Return on CR

          if (termMode && pos <=0){
              buffer[pos++] = char('<');
              rpos = pos;
              pos = 0;// Reset position index ready for next time
              return rpos;
          }
          rpos = pos;
          pos = 0;// Reset position index ready for next time
          return rpos;
      default:
           if(termMode){
              command_line += String(char(readch));
            }
            buffer[pos++] = readch;
            buffer[pos] = 0;
      }
  }

  // No end of line has been found, so return -1.
  return NULL;
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
char serialRead(){
  
    if(Serial.available() > 0){ 
    char in = Serial.read();
    return in;} else {return NULL;}
}
void serialPrint(String data){
    Serial.print(data);
    Serial.flush();

}
void serialPrintln(String data){
    Serial.println(data);
    Serial.flush();
}
FileType getFileType(String filename){

  String upper = filename;
  upper.toUpperCase();
  if (upper.endsWith(".TXT"))
  {
    return TXT;
  }
  if (upper.endsWith(".ROM"))
  {
    return ROM;
  }
  if (upper.endsWith(".DEV"))
  {
    return DEV;
  }
  if (upper.endsWith(".MD5"))
  {
    return MD5;
  }
  if (upper.endsWith(".INI"))
  {
    return INI;
  }
return ROM;
}

void encoderBegin(){
    attachInterrupt(digitalPinToInterrupt(ENCSW), select, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCA), rotate, FALLING);
}
void encoderEnd(){
    detachInterrupt(digitalPinToInterrupt(ENCSW));
    detachInterrupt(digitalPinToInterrupt(ENCA));
}
void select(){
    byte state = digitalRead(ENCSW);
    EncoderSwitchState = state ? UP : DOWN;
    EncoderSWInt = true;
}


void rotate(){
   byte dir = digitalRead(ENCB);
   EncoderDirection =  dir ? FORWARD : BACKWARD;
   EncoderDIRInt = true;
}


String readPROGMEMtoString(const char * conchr){ 

    
    uint8_t i=0, j=0;
    char myChar;
    String cd_items = "";
    for (byte k = 0; k < strlen_P(conchr); k++) {
       cd_items += myChar = pgm_read_byte_near(conchr + k);
    }    
    return cd_items;
       
}

String getDateTime(){
      
        String out = "";
        out = DOW[t.wday -1];
        out += " ";
        out += String(t.mday);
        out += " ";
        out += MOY[t.mon -1];
        out += " ";
        out += String( t.year);
        out += " ";
        out += String(t.hour);
        out += ":";
        if (t.min < 10) out += "0";
        out += String(t.min);
        out += ":";
        if (t.sec < 10) out += "0"; 
        out += String(t.sec);

        return out;
      
      
 }
