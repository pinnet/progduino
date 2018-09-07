SDStatus MountSD(){
  
    if(card.init(SPI_HALF_SPEED, chipSelect)){
      return  SDS_MOUNTED;
    }
    else{
      return  SDS_UNMOUNTED;
    }
}


String volSize(byte unit){
  
  if(!volume.init(card)){
    cardStatus = SDS_VOLERROR;
    return "No Volume";
  }
  uint32_t volumesize = volume.blocksPerCluster();// clusters are collections of blocks
  uint32_t freespace = volume.blocksPerCluster();
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;  // SD card blocks are always 512 bytes 
  
  
  if( unit == char('G')) {
    volumesize /= 1024;
    volumesize /= 1024;
    volumesize /= 1024;
    freespace /= 1024;
    freespace /= 1024;
    freespace /= 1024;
    return String(volumesize) + " GB";
    }
  else if( unit == char('M')) {
    volumesize /= 1024;
    volumesize /= 1024;
    return String(volumesize) + " MB";
    }
   else if (unit ==  char('K')){
    volumesize /= 1024;
    return String(volumesize) + " KB";
    }
    else {
    return String(volumesize) + " bytes";
    }  
}

void SaveBuffer(unsigned long adr, unsigned int size,File myFile){
 char b = 0;
 if (myFile) {
  
 // myFile.print(hex[ (currentPage  & 0xF0) >> 4]);
 // myFile.print(hex[ (currentPage & 0x0F) ]);
 // myFile.print(hex[ (adr  & 0xF0) >> 4]);
 // myFile.print(hex[ (adr & 0x0F) ]);
 // myFile.print(":");
  
 // byte chk = 0;
  for (unsigned long x = adr; x <  adr + 16; x++)
  {
  b = pageBuff[x];
  myFile.write(b);
  // myFile.print(hex[ (pageBuff[x] & 0xF0) >> 4 ]);
  // myFile.print(hex[ (pageBuff[x] & 0x0F)      ]); 
  }
//  myFile.print("=");
//  myFile.print(hex[ (chk & 0xF0) >> 4 ]);
//  myFile.print(hex[ (chk & 0x0F)      ]);
 // myFile.println("");
  
    
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }

}
void printDirectory(File dir,int dpage,DirMode mode) {

  int pg = dpage;

  int index = 1;  

   for(int i =0;i < dpage;i++){
      for (int l = 0; l <= 14; l ++){
          File entry =  dir.openNextFile();
      }
   }
   if(mode == INFO) Serial.println(F("\t\tPart Number\t\tDescription"));
   if(mode == LIST) Serial.println(F("\t\t\tFilename\tSize"));
   for (int lines = 0; lines < 14; lines ++){

    File entry =  dir.openNextFile();
    if (! entry) {
      if (pg > 0) {
          if (termMode){ 
            term.position(18,0);
            term.set_attribute(BT_REVERSE);
            Serial.print(F("\t\t        0: << Back                       "));
            term.set_attribute(BT_NORMAL);
            term.set_attribute(BT_BOLD);
             }
           }     
      // no more files
      break;
    }
    String fname = entry.name();
     
        if (!entry.isDirectory()) {

           if(mode == LIST){ Serial.print("\t\t\t");}
           else{             Serial.print("\t\t");}
          
          Serial.print(index ++,HEX);
          Serial.print(":");
          curdir[index -1] = fname;
          
          
            if(mode == LIST){ 
                              Serial.print(fname);
                              Serial.print("\t");
                              Serial.print(entry.size(), DEC);
                              Serial.println(" Bytes");}

            else if(mode == INFO){ 
                              char b = '.';
                              myFile=SD.open("/DEVICE/"+fname);
                              while(myFile.available()){
                                
                                b = myFile.read();
                                if (b != '\n'){
                                      Serial.write(b);}
                                else {
                                  break;}
                                 
                              }

                              Serial.println(".");
                              }
                
            else{             Serial.println(".");}
          
          }else { lines -- ; }
       
       if (index > 14){
        if (termMode){ 
          term.position(18,0);
          term.set_attribute(BT_REVERSE);
          Serial.print(F("\t\t        F: More...    "));
          if (pg > 0) {
             Serial.print(F("  0: << Back        "));
          }
          else {
             Serial.print(F("                    "));
          }
          term.set_attribute(BT_NORMAL);
          term.set_attribute(BT_BOLD);
        }  
        curdir[lines] = "NEXT";
      }  
      entry.close();
   }
   
}
void GetFilePage(int page){
      
      ReadFileIntoBuffer((page * pageSize), pageSize);  
}
void ReadFileIntoBuffer(unsigned long addr, int size)
{
  digitalWrite(PIN_LED_GREEN, HIGH);
  byte b;
  //char out2; 
 
  myFile=SD.open(filename);
  
  for ( int i= 0;i < size;i++){

 myFile.seek(addr); 
 b = myFile.read();
   pageBuff[i] = b;
   // out2 = myFile.read();

    // if(out1 == '\r'){
    //   out1 = myFile.read();
    //   out2 = myFile.read();
    //   addr += 2;
     // }
    // char b[2];    
    // b[0]=out1;
    // b[1]=out2;
    
     //pageBuff[i] = strtoul(out1,NULL,16) ;
     addr += 1;
  }

  myFile.close();
  digitalWrite(PIN_LED_GREEN, LOW);
}

void verify(){
  unsigned int addr =0;
  static int size = pageSize;
  byte b = 0;
  myFile=SD.open(filename);
  
  digitalWrite(PIN_LED_GREEN, HIGH);
   for (unsigned int a = 0 ; a <= (romSize / 256) ; a ++){
    
      currentPage = a ; 
      GetRomPage(a);
      
      for ( int i= 0;i < size;i++){

        myFile.seek(addr); 
        b = myFile.read();
     if (pageBuff[i] != b){
      Serial.print(F("\n!! verify fail !! Page "));
      Serial.print(a,DEC);
      Serial.print("-");
      Serial.print(i,HEX);
      Serial.print(":");
      Serial.print(pageBuff[i],HEX);
      Serial.print("!=");
      Serial.println(b,HEX);
        
      digitalWrite(PIN_LED_GREEN, LOW); 
      myFile.close();
      return;
     }
     addr ++;
    }
  }
  myFile.close();
  digitalWrite(PIN_LED_GREEN, LOW); 
  Serial.print(F("\n!! verify pass !! "));
}


