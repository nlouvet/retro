#include <SPI.h>
#include "winbondflash.h"

SPIClass spi(PA7, PA6, PA5);
winbondFlashSPI mem(spi, PA4);

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.print("Init W25Q32 spi flash: ");
  if(mem.begin(winbondFlashClass::W25Q32))
    Serial.println("OK");
  else {
    Serial.println("FAILED");
    while(1);
  }

  int addr;
  char buf1[256] = "Toto fait du velo !!!";
  char buf2[256] = "Tutu fait du pedalo :D";
 #if 0
  while(mem.busy());
  mem.setWriteEnable(true);
  mem.eraseSector(0);
  Serial.println("OK");

  while(mem.busy());
  mem.setWriteEnable(true);
  mem.writePage(512, (uint8_t*)buf1);
  Serial.println("OK");

  while(mem.busy());
  mem.setWriteEnable(true);
  mem.writePage(256, (uint8_t*)buf2);
  //while(mem.busy());
  //mem.setWriteEnable(false);
  Serial.println("OK");
#endif
}

void loop() {
  char buf[256];
  while(mem.busy());
  mem.read(256, (uint8_t*)buf);
  Serial.println(buf);

  while(mem.busy());
  mem.read(257, (uint8_t*)buf, 256);
  Serial.println(buf);
  Serial.println("OK");

  delay(1000);

  /*
  char buf[256];

  while(mem.busy());
  mem.read(0, (uint8_t*)buf, 256);
  Serial.println(buf);

  while(mem.busy());
  mem.read(256, (uint8_t*)buf, 256);
  Serial.println(buf);
  */

/*
  if(Serial.available() > 0)
  {
    switch(Serial.read())
    {
    case 'R':
      {
        //R0,100
        Serial.print(("Read "));
        int addr = Serial.parseInt();
        int len = Serial.parseInt();
        Serial.print(("addr=0x"));
        Serial.print(addr>>8,HEX);
        Serial.print(addr,HEX);
        Serial.print((",len=0x"));
        Serial.print(len>>8,HEX);
        Serial.print(len,HEX);
        Serial.println((":"));
        uint8_t *buf = new uint8_t[len];
        while(mem.busy());
        mem.read(addr,buf,len);
        for(int i = 0;i < len; i++)
        {
          Serial.print((char)buf[i]);
        }
        Serial.println();
        Serial.println(("OK"));
        delete [] buf;
      }
      break;
    case 'W':
      {
        //W0,test string[LF]
        //
        Serial.print(("Write "));
        int addr = Serial.parseInt();
        Serial.print(("0x"));
        Serial.print(addr>>8,HEX);
        Serial.print(addr,HEX);
        Serial.read();
        uint8_t buf[256];
        uint8_t len = Serial.readBytesUntil('\n',(char*)buf,256);
        Serial.print((",0x"));
        Serial.print(len>>8,HEX);
        Serial.print(len,HEX);
        Serial.print((": "));
        while(mem.busy());
        mem.WE();
        mem.writePage(addr,buf);
        Serial.println(("OK"));
      }
      break;
    case 'E':
      while(Serial.available() < 1);
      char x = Serial.read();
      //Serial.print(x);
      switch(x)
      {
        int addr;
      case 'S':
        Serial.print(("Erase Sector "));
        addr = Serial.parseInt();
        Serial.print(("addr=0x"));
        Serial.print(addr>>8,HEX);
        Serial.print(addr,HEX);
        Serial.print((": "));
        mem.WE();
        mem.eraseSector(addr);
        Serial.println("OK");
        break;
      case 'b':
        Serial.print(("Erase 32k Block "));
        addr = Serial.parseInt();
        Serial.print(("addr=0x"));
        Serial.print(addr>>8,HEX);
        Serial.print(addr,HEX);
        Serial.print((": "));
        mem.WE();
        mem.erase32kBlock(addr);
        Serial.println("OK");
        break;
      case 'B':
        Serial.print(("Erase 64k Block "));
        addr = Serial.parseInt();
        Serial.print(("addr=0x"));
        Serial.print(addr>>8,HEX);
        Serial.print(addr,HEX);
        Serial.print((": "));
        mem.WE();
        mem.erase64kBlock(addr);
        Serial.println("OK");
        break;
      case 'A':
        while(mem.busy());
        mem.WE();
        mem.eraseAll();
        long ss = millis();
        while(mem.busy())
        {
          Serial.print(millis()-ss);
          delay(1000);
        }
        break;
      }
    } 
  }
  */
}


      /*
      if(Serial.read() == 'S')
       {
       //ES0
       Serial.print(("Erase Sector "));
       int addr = Serial.parseInt();
       Serial.print(("addr=0x"));
       Serial.print(addr>>8,HEX);
       Serial.print(addr,HEX);
       Serial.print((": "));
       mem.WE();
       mem.eraseSector(addr);
       Serial.println("OK");
       }
       break;
       */
