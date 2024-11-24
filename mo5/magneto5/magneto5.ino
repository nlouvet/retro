#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "serial_packets.hpp"
#include "magneto.hpp"

#define LEDPIN 5

/*
  SD card attached to SPI bus as follows:
  SDO - pin 11
  SDI - pin 12
  CLK - pin 13
  CS  - SDCS depends on your SD card shield or module.
 	Pin 10 used here for consistency with other Arduino examples (for MKR Zero SD: SDCARD_SS_PIN)
*/
#define SDCS 10

void failure(const __FlashStringHelper* msg) {
  Serial.print(F("FAILURE: "));
  Serial.println(msg);
  while(1);
}

void setup() {
  byte buffer[320];
  File fd;

  Serial.begin(115200);
  while (!Serial);

  pinMode(LEDPIN, OUTPUT);

  Serial.println("Initializing SD card...");
  if(!SD.begin(SDCS)) failure(F("initialization failed."));
  Serial.println(F("Initialization done."));

  if(SD.exists("tmp.k7")) {
    if(!SD.remove("tmp.k7")) failure(F("removing temporary file failed."));
  }

  fd = SD.open("tmp.k7", FILE_WRITE); 
  if(!fd) failure(F("removing temporary file failed."));
  else Serial.println(F("Temporary file opened!"));

  Serial.println(F("READY"));

  int32_t size, nbrcv = 0;
  int16_t l;

  String str = Serial.readStringUntil('\n');
  size = str.toInt();

  Serial.print(F("waiting to receive "));
  Serial.print(size);
  Serial.println(F(" bytes"));

  send_ready();
  while(nbrcv != size) {
    l = recv_packet(buffer);
    fd.write(buffer, l);
    nbrcv += l;
    send_ack();
  }

  fd.close();

  fd = SD.open("tmp.k7", FILE_READ);
  if(!fd) Serial.println(F("Unable to open the file..."));
  else Serial.println(F("File opened!"));

  magneto M(fd);
  M.read_k7();

  Serial.println(F("END"));

  fd.close();
}

void loop() {
  digitalWrite(LEDPIN, LOW);
  delay(100);
  digitalWrite(LEDPIN, HIGH);
  delay(100);
}
