// ---------------------------------------------------------------------------------
// DO NOT USE CLASS-10 CARDS on this project - they're too fast to operate using SPI
// ---------------------------------------------------------------------------------
/*
                                      TZXduino
                               Written and tested by
                            Andrew Beer, Duncan Edwards
                            www.facebook.com/Arduitape/

                Designed for TZX files for Spectrum (and more later)
                Load TZX files onto an SD card, and play them directly
                without converting to WAV first!

                Directory system allows multiple layers,  to return to root
                directory ensure a file titles ROOT (no extension) or by
                pressing the Menu Select Button.

                Written using info from worldofspectrum.org
                and TZX2WAV code by Francisco Javier Crespo

 *              ***************************************************************
                Menu System:
                  TODO: add ORIC and ATARI tap support, clean up code, sleep

                V1.0
                  Motor Control Added.
                  High compatibility with Spectrum TZX, and Tap files
                  and CPC CDT and TZX files.

                  V1.32 Added direct loading support of AY files using the SpecAY loader
                  to play Z80 coded files for the AY chip on any 128K or 48K with AY
                  expansion without the need to convert AY to TAP using FILE2TAP.EXE.
                  Download the AY loader from http://www.specay.co.uk/download
                  and load the LOADER.TAP AY file loader on your spectrum first then
                  simply select any AY file and just hit play to load it. A complete
                  set of extracted and DEMO AY files can be downloaded from
                  http://www.worldofspectrum.org/projectay/index.htm
                  Happy listening!
*/

#include "SdFat.h"
#include "TZXProcessing.h"

#define SPI_CS 10

//Set defines for various types of screen, currently only 16x2 I2C LCD is supported
#define SERIALSCREEN  1

#include <Wire.h>

SdFat sd;                           //Initialise Sd card
File file;                       //SD card file

const int chipSelect = 10;          //Sd card chip select pin

void setup() {
  Serial.begin(115200);

  pinMode(chipSelect, OUTPUT);      //Setup SD card chipselect pin
  if(!sd.begin(SPI_CS, SPI_FULL_SPEED)) {
    // Start SD card and check it's working
    Serial.println("No SD Card!");
    delay(100);
  }

  sd.chdir();                       //set SD to root directory
  TZXSetup(&sd);                    //Setup TZX specific options

  Serial.println("TZXDuino v1.7");
  delay(2000);

  Serial.println("Ready..");

  TZXPlay("default.cdt");           //Load using the short filename
}

void loop(void) {
    TZXLoop();
}
