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
void dateTime(uint16_t* date, uint16_t* time)
{
  *date = FAT_DATE(t.year, t.mon, t.mday);
  *time = FAT_TIME(t.hour, t.min, t.sec);
  
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
    printerror("FILE","error opening file");
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
   if(mode == LIST) Serial.println(F("\t\tFilename\tModifed\t\tSize"));
   for (int lines = 0; lines < 14; lines ++){

    File entry =  dir.openNextFile();
    if (! entry) {
      if (pg > 0) {
          if (termMode){ 
            term.position(18,0);
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

           if(mode == LIST){ Serial.print("\t\t");}
           else{             Serial.print("\t\t");}
          
          Serial.print(index ++,HEX);
          Serial.print(":");
          //Serial.print(fname);
          //Serial.print(index,HEX);
          
          
            if(mode == LIST){ 
                              Serial.print(fname);
                              //Serial.print("\t");
                              //Serial.print(entry.name());
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
      pauseInt(true);
      ReadFileIntoBuffer((page * pageSize), pageSize);  
      pauseInt(false);
}
void ReadFileIntoBuffer(unsigned long addr, int size)
{

  byte b;
 
  myFile=SD.open(filename);

  
  for ( int i= 0;i < size;i++){

 myFile.seek(addr); 
 b = myFile.read();
   pageBuff[i] = b;
     addr += 1;
  }

  myFile.close();

}




