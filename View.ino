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
    if(termMode){
      Serial.println("\t" + pad2center("EEprom Master "+ Version + " (c) Danny Arnold 2017",56,"-"));                      //  header
      Serial.println("\t" + pad2center(line,56," "));
      Serial.println(F("\t-------------------------------------------------------- "));
    } else {
      Serial.println(line);
    }
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
    term.set_color(filePageForeground,filePageBackground);
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
    oledOut("Error","initialization failed!");
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
    if(termMode){
      Serial.println(F("\t-------------------------------------------------------- "));                       //   footer
      Serial.println("\t" + pad2center(line,58," "));
      Serial.println(F("\t-------------------------------------------------------- "));
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
  Serial.print(F("\r\n\tRom Master by Danny Arnold (2017) firmware version "));
  Serial.println(Version + "\r\n");
  Serial.println(F("-------------------------------------------------------------------------------- ")); 
  /*
  for (int i = 0; i < 7; i++){
    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); 
    Serial.print(buffer);           
  }
  Serial.println(F("Help\t\t\t?\tThis help page\r\n"));
  
  */
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
void blankpage(){   // why?

  term.position(3,0);
  for (byte ln=0;ln < 16;ln ++){
    
    for (int space =0;space < 80;space ++){
      //Serial.print('.');      
    }
    //Serial.println();
    
  }
  term.position(3,0);
}

void printinfo(String title,String message){
    count = 0;
    oledOut(title,message);
    if (termMode){
        Serial.println( title +" "+ message+ "                    ");
    }



}