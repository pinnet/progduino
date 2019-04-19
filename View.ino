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


void setpage(){
    if (cmd[1].toInt() >= 0){
      currentPage = cmd[1].toInt(); 
      if((currentPage * 256) >= romSize){ currentPage = 0;}
      GetPage();
      DisplayCurrentPage();
    } else { DisplayCurrentPage(); }
}

void resetpage(){
  currentPage = 0;
        curpos = 0; 
        if(termMode) DisplayCurrentPage();
}
void cont(){
 if (curpos + chunkSize > 256){
              currentPage ++;
              if((currentPage * 256) >= romSize){ currentPage = 0;}
            }
            PrintBuffer(curpos,chunkSize,true);

}
void lastpage(){

    currentPage --; 
    if((currentPage * 256) >= romSize){ currentPage = 0;}
    DisplayCurrentPage();   
}
void nextpage(){

   currentPage ++;
   if((currentPage * 256) >= romSize ){ currentPage = 0;}
   DisplayCurrentPage();
}
void swappages(){
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
        line = "Device : "+ devicetype +"  : Page " + String(currentPage) + " / "+ String((romSize /256)) +" : Size "+ String(romSize)+" bytes";

        oledOut("Device",devicetype);
  }
  else if (currentMode == MODE_FILE){
       if (termMode)  term.set_color(F_headerForeground,F_headerBackground);
        line = "File : " + filename + " : Page " + String(currentPage) + " / "+ String((romSize /256)) + " : "; 
        line += (cardStatus != SDS_MOUNTED) ?  "Not Mounted" : "file count";

        oledOut("File",filename);
   }
   else if (currentMode == MODE_BOOT){
     oledOut("Eeprom Master "+ Version,"(c) Danny Arnold 2017");
     line = "? = help for more infomation";
    }

  if (termMode) term.cls();   
  if (line != ""){
      if(banners){
      Serial.println("\t  " + pad2center(" EEprom Master "+ Version + " (c) Danny Arnold 2017 ",60,"-"));                      //  header
      Serial.println("\t  " + pad2center(line,56," "));
      Serial.println(F("\t  ----------------------------------------------------------- "));
      }
  }
 //---------------------------------------------------------------------------------------------------------------------------------------------------------

if (view == VIEW_ROM){
  if (termMode) {
    term.set_color(romPageForeground,romPageBackground);
    
  }
  displaybuff(pageBuff,0,pageSize,ROM);
}
else if (view == VIEW_DIRECTORY){
  if (termMode){
    term.set_color(filePageForeground,filePageBackground);
  }
  myFile = SD.open("/");
  myFile.rewindDirectory();
  printDirectory(myFile,dPage,LIST);
  if (termMode)term.position(19,0);  
}
else if (view == VIEW_FILE){
  if (termMode){
    term.set_color(filePageForeground,filePageBackground);
     
  }
  displaybuff(pageBuff,0,pageSize,getFileType(filename));    
}
else if (view == VIEW_INFO){
  if (termMode){
    term.set_color(romPageForeground,romPageBackground);  
  }
   myFile = SD.open("/DEVICE/");
   myFile.rewindDirectory();
   printDirectory(myFile,dPage,INFO); 
}
else if (view == VIEW_BOOT){

    if (!SD.begin(24)) {
    oledOut("Error","initialization failed!");
    Serial.println("initialization failed!");
    while(1){};
    }
    else{

      filename = "Readme.txt";
     
      displaybuff(pageBuff,0,pageSize,TXT);
       
      filename = lastfilename; 
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
      line = "[p (XXX)] = goto page: [>] = next: [<] = back: [h] = help";
  }
  else if (view == VIEW_FILE){
        if (termMode) term.set_color(F_footerForeground,F_footerBackground);
        line = "[p (XXX)] = goto page: [>] = next: [<] = back: [h] = help";
  }
   else if (view == VIEW_DIRECTORY){
        if (termMode) term.set_color(F_footerForeground,F_footerBackground);
        line = "[l] = List Dir : Choose [0 - F] : [l (filename)] = Load File";
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
      if(banners){
        Serial.println(F("\t------------------------------------------------------------ "));                       //   footer
        Serial.println("\t" + pad2center(line,58," "));
        Serial.println(F("\t------------------------------------------------------------ "));
        }
  } 
}
void HelpPage(){
  char buffer[165];
  if (termMode){
    term.set_color(helpPageForeground,helpPageBackground);
    term.cls();
    term.position(0,0);
  }
  Serial.print(F("\r\n\tEEprom Master by Danny Arnold (2017) firmware version "));
  Serial.println(Version + "\r\n");
  Serial.print(F("----------------------------- Help page --------------------------------------- ")); 

  lastfilename = filename; 
  filename = "Help.txt";  
  displaybuff(pageBuff,0,pageSize,TXT);  
  if (currentPage >= 2){
    currentPage = 0;
  }   
  filename = lastfilename; 
}
void SettingsPage(){
  char buffer[165];
  if (termMode){
    term.set_color(settingsPageForeground,settingsPageBackground);
    term.cls();
    term.position(0,0);
  }
  Serial.print(F("\r\n\tEEprom Master by Danny Arnold (2017) firmware version "));
  Serial.println(Version + "\r\n");
  Serial.print(F("---------------------------- Settings page ------------------------------------ ")); 
  lastfilename = filename; 
  filename = "Settings.ini";  
  displaybuff(pageBuff,0,pageSize,TXT);     
  filename = lastfilename; 
}
void AboutPage(){
  char buffer[165];
  if (termMode){
    term.set_color(settingsPageForeground,settingsPageBackground);
    term.cls();
    term.position(0,0);
  }
  Serial.print(F("\r\n\tEEprom Master by Danny Arnold (2017) firmware version "));
  Serial.println(Version + "\r\n");
  Serial.print(F("---------------------------- About page ------------------------------------ ")); 
  lastfilename = filename; 
  filename = "Readme.txt";  
  displaybuff(pageBuff,0,pageSize,TXT);     
  filename = lastfilename; 
}
static void displaybuff(byte *epm,word address,word datalength,FileType type)
{
  switch(type){
    case ROM :
          dumpHex(epm,address,datalength);  
      break;
    case TXT :
          dumpTxt(epm,address,datalength);
      break;
  }
}
void dumpHex(byte *epm,word address,word datalength){

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
      serialPrintln(lbuf);   
    }
}
void dumpTxt(byte *epm,word address,word datalength){
    char lbuf[81];
    char *x;
    char byt;
    

  myFile=SD.open(filename);

  for(int l =0 ;l < 80*16;l += 80){
    x=lbuf;
    for ( int i= 0;i < 80;i++){
        myFile.seek(l + i + (currentPage * 80*16)); 
        byt = myFile.read();
        switch (byt){
          case '\t' :
            *x=' ';
            *x++=' ';
            *x++=' ';
            *x++=' ';
            
            break;
          case '\r' :
           *x='\b';
           break;  
          case '\n' :
           *x='\b';
            break;
          default   :
            if (byt >=32 && byt <= 127) *x=byt;
            else *x=byt + 32;
            break;
        }
      x++;
    }
    *x=0;
    
    serialPrintln(lbuf);
  
 }
}

void printinfo(String title,String message){
    count = 0;
    oledOut(title,message);
    if (termMode){
        Serial.println( " " + title +" "+ message+ "                    ");
    }



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