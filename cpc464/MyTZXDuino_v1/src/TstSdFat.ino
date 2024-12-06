#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "TZXProcessing.h"
#include "RotaryEncoder.h"
#include "PushButton.h"
#include "SdFat.h"
#include <Wire.h>

// SPI micro SD card reader, Arduino Nano:
// SCK -> D13
// MISO -> D12
// MOSI -> D11
// CS -> D10
#define SD_CARD_CS 10

// SSD1306, I2C, 128x64 oled, Arduino Nano:
// SDA -> A4
// SCK -> A5
#define OLED_I2C_ADD 0x3C

#define ROTENC_IN1  A2
#define ROTENC_IN2  A3

SdFat sd;     //Initialise Sd card
File entry;   //SD card file
File root;

SSD1306AsciiAvrI2c oled;
RotaryEncoder rotenc(ROTENC_IN1, ROTENC_IN2, RotaryEncoder::LatchMode::TWO03);
PushButton rotenc_butt(A1);

uint16_t minIndex = 0;
uint16_t maxIndex = 0;
uint16_t currIndex = 0;
uint16_t numFiles = 0;
char file_name[32];

int rotenc_cur = 0;
int rotenc_prv = 0;

bool updateCurrIndex(void);
void displayCurrIndex(void);

enum {
  stStart,
  stStopped,
  stBrowsing,
  stStartPlaying,
  stPlaying,
} state;

void setup() {
  Serial.begin(115200);

  oled.begin(&Adafruit128x64, OLED_I2C_ADD);
  //oled.begin(&SH1106_128x64, OLED_I2C_ADD);
  oled.setFont(System5x7);

  oled.clear();
  oled.println(F("Initialization..."));

  pinMode(SD_CARD_CS, OUTPUT);      //Setup SD card chipselect pin
  if(!sd.begin(SD_CARD_CS, SPI_FULL_SPEED)) {
    // Start SD card and check it's working
    oled.println(F("No SD card!"));
    while(true);
  }

  oled.println(F("SD card init'ed!"));

  TZXSetup(&sd);

  oled.println(F("TZX player init'ed!"));

  bool first_file = true;
  root.open("/");
  root.rewindDirectory();
  while(entry.openNext(&root, O_RDONLY)) {
    if(entry.isFile() && !entry.isHidden()) {
      if(first_file) {
        first_file = false;
        minIndex = root.curPosition() / 32 - 1;
        maxIndex = minIndex;
      }
      else {
        maxIndex = root.curPosition() / 32 - 1;
      }
      numFiles++;
    }
    entry.close();
  }
  root.rewindDirectory();
  if(first_file) {
    oled.println(F("No file found..."));
    while(true);
  }

  oled.println(F("SD card listed!"));
  oled.print(F("first pos: ")); oled.println(minIndex);
  oled.print(F("last pos:  ")); oled.println(maxIndex);
  oled.print(F("num files: ")); oled.println(numFiles);

  // the entry maxIndex at the root directory will be the first displayed
  currIndex = maxIndex;
  state = stStart; 

  delay(3000);
}

void loop(void) {
  rotenc.tick();
  rotenc_butt.tick();
  rotenc_cur = rotenc.getPosition();

  if(state == stStart) {
    displayCurrIndex();
    state = stBrowsing;
  }

  if(state == stBrowsing) {
    if(updateCurrIndex())
      displayCurrIndex();
    if(rotenc_butt.isPressed())
      state = stStartPlaying;
  }

  if(state == stStartPlaying) {
    entry.open(currIndex, O_RDONLY);
    entry.getName(file_name, 32);
    entry.close();

    oled.clear();
    oled.println(file_name);
    oled.println(F("---------------------"));
    TZXPlay(file_name);
    oled.println(F("Playing..."));
    state = stPlaying;
  }

  if(state == stPlaying) {
    if(!TZXLoop()) {
      byte err = TXZGetErrorCode();
      if(err != ERROR_NONE) {
        oled.print(F("ERROR "));
        oled.println(err);
      }
      else {
        oled.println(F("Finished!"));
      }
      oled.println(F("(press to cont')"));
      state = stStopped;
    }
    if(rotenc_butt.isPressed()) {
      TZXStop();
      oled.println(F("Player stopped..."));
      oled.println(F("(press to cont')"));
      state = stStopped;
    }
  }

  if(state == stStopped) {
    if(rotenc_butt.isPressed()) state = stStart;
  }

}

/* -------------------------------------------------- */

bool updateCurrIndex(void) {
  uint16_t newIndex;

  if(rotenc_cur != rotenc_prv) {
    if(rotenc_cur > rotenc_prv) {
      if(currIndex < maxIndex) {
        // searching for the next available file in the directory
        // since currIndex < maxIndex, we will find a file
        for(newIndex = currIndex + 2; newIndex <= maxIndex; newIndex += 2) {
          if(entry.open(newIndex, O_RDONLY) && entry.isFile() && !entry.isHidden()) {
            currIndex = newIndex;
            entry.close();
            break;
          }
        }
      }
    }
    else { // rotenc_cur < rotenc_prv
      if(currIndex > minIndex) {
        // searching for the previous file available in the directory
        // since currIndex > maxIndex, we will find a file
        for(newIndex = currIndex - 2; newIndex >= minIndex; newIndex -= 2) {
          if(entry.open(newIndex, O_RDONLY) && entry.isFile() && !entry.isHidden()) {
            currIndex = newIndex;
            entry.close();
            break;
          }
        }
      }
    }
    rotenc_prv = rotenc_cur;
    return true;
  }
  return false;
}

void displayCurrIndex(void) {
  uint32_t size;

  entry.open(currIndex, O_RDONLY);
  entry.getName(file_name, 32);
  size = entry.fileSize();
  entry.close();

  oled.clear();
  oled.println(file_name);
  oled.println(F("---------------------"));
  oled.print(F("size: ")); oled.print(size); oled.println(" bytes");
  oled.print(F("pos: ")); oled.println(currIndex);

}
