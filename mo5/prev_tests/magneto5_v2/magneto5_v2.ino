#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define MOTOPIN 3
#define DATAOUT 4
#define LEDPIN 5

#define CHIPSEL 10
/*
  SD card attached to SPI bus as follows:
  SDO - pin 11
  SDI - pin 12
  CLK - pin 13
  CS - depends on your SD card shield or module.
 	Pin 10 used here for consistency with other Arduino examples (for MKR Zero SD: SDCARD_SS_PIN)
*/

// file type: 00=Basic 01=Data 02=Binaire
#define BASIC_FILE  0x00
#define DATA_FILE   0x01
#define BINARY_FILE 0x02
// file mode: 00=Binaire FF=Texte
#define BINARY_MODE 0x00
#define TEXT_MODE   0xFF

// File desciptor for reading the SD card.
File fd;

// Current state (0 or 1) of the DATOUT pin.
byte state = 0x00;

// Shortest delay in microseconds between two
// state changes of the DATOUT pin
#define TOGGLE_DELAY 395

// Delay beetween each block of a file;
// 160 ms seems fine for binary files
// and tokenized basic files, but this
// must be increased to at least 1500 ms
// for ASCII text basic files (to allow
// each line to be proceeded).
#define BINARY_MODE_DELAY 160 // ms
#define TEXT_MODE_DELAY 1500   // ms
uint16_t inter_block_delay = BINARY_MODE_DELAY;

#define BUFLEN 320
byte buffer[BUFLEN];

void failure(const __FlashStringHelper* msg) {
  Serial.print(F("FAILURE: "));
  Serial.println(msg);
  while(1);
}

void reset_datout() {
  state = 0x01;
  digitalWrite(DATAOUT, state);
}

void toggle_datout() {
  state ^= 0x01;
  digitalWrite(DATAOUT, state);

}

int16_t load_header_block(byte *buf);
int16_t load_block(byte *buf);
bool is_end_block(byte *buf);


void read_block(byte *buf, uint16_t len) {
  for(uint16_t i = 0; i < len; i++) {
    byte w = buf[i];
    for(uint8_t j = 0; j < 8; j++) {
      toggle_datout();
      delayMicroseconds(TOGGLE_DELAY);
      if(w & 0x80) toggle_datout();
      delayMicroseconds(TOGGLE_DELAY);
      w <<= 1;
    }
  }
  reset_datout();
}

int8_t pausems(uint16_t dt) {
  static uint32_t tstamp = 0;
  if(dt == 0)
    tstamp = millis();
  else {
    if(millis() - tstamp > dt)
      return -1;
    else
      while(millis() - tstamp <= dt);
  }
  return 1;
}

/*
void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(DATAOUT, OUTPUT);
  pinMode(MOTOPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);

  Serial.println("Initializing SD card...");

  if (!SD.begin(CHIPSEL)) failure(F("initialization failed."));
  Serial.println(F("Initialization done."));

  //if(!SD.exists("assdesass_mo5.k7")) failure(F("file not found."));
  //Serial.println(F("File found."));

  reset_datout();

  Serial.println(F("Trying to open the file..."));
  fd = SD.open("test.k7", FILE_READ);
  if(!fd) failure(F("error on opening file"));

  int16_t nbrd = 0;

  Serial.println(F("Trying to load block 0"));
  nbrd = load_header_block(buffer);
  if(nbrd == -1) failure(F("error while loading a block"));
  Serial.println(F("Block 0 loaded"));
  Serial.println();
  Serial.println();

  while(digitalRead(MOTOPIN) == HIGH);
  digitalWrite(LEDPIN, HIGH);
  Serial.println(F("Start reading..."));

  delay(2500);

  read_block(buffer, nbrd);

  delay(2340);

  uint16_t i = 1;
  pausems(0);
  while(fd.available()) {
    Serial.print(F("Loading block "));
    Serial.println(i);
    nbrd = load_block(buffer);
    if(nbrd == -1) failure(F("error while loading a block"));
    if(pausems(inter_block_delay) == -1) failure(F("error delay exceeded"));
    //if(digitalRead(MOTOPIN) == HIGH) {
    //    while(digitalRead(MOTOPIN) == HIGH);
    //    delay(2340);
    //}
    read_block(buffer, nbrd);
    i++;
    pausems(0);
  }

  Serial.println(F("End reading..."));

  fd.close();
}
*/

void print_block(byte *buf, uint16_t len) {
  char tmp[4];
  uint16_t i;
  for(i = 0; i < len; i++) {
    if(buf[i] < 16) Serial.print(" ");
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(DATAOUT, OUTPUT);
  pinMode(MOTOPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);

  Serial.println("Initializing SD card...");

  if (!SD.begin(CHIPSEL)) failure(F("initialization failed."));
  Serial.println(F("Initialization done."));

  //if(!SD.exists("assdesass_mo5.k7")) failure(F("file not found."));
  //Serial.println(F("File found."));

  reset_datout();

  Serial.println(F("Trying to open the file..."));
  fd = SD.open("BILLYBOX.k7", FILE_READ);
  if(!fd) failure(F("error on opening file"));

  int16_t nbrd = 0;
  uint16_t i = 0;
  bool cont = 1;

  do {

    Serial.print(F("Trying to load header block "));
    Serial.println(i);
    nbrd = load_header_block(buffer);
    if(nbrd == -1) failure(F("error while loading a header block"));
    //print_block(buffer, nbrd);
    while(digitalRead(MOTOPIN) == HIGH);
    Serial.println(F("Reading file..."));
    delay(2500);
    read_block(buffer, nbrd);
    delay(2500);

    pausems(0);
    do {
      Serial.print(F("Loading block "));
      Serial.println(i);
      nbrd = load_block(buffer);
      if(nbrd == -1) failure(F("error while loading a block"));
      //print_block(buffer, nbrd);
      cont = !is_end_block(buffer);
      if(pausems(inter_block_delay) == -1) failure(F("error delay exceeded"));
      read_block(buffer, nbrd);
      i++;
      pausems(0);
    } while(cont);

    Serial.println(F("Stop reading file..."));
    delay(2500);
  } while(fd.available());

  Serial.println(F("End of the k7 reached..."));

  fd.close();
}


void loop() {
  digitalWrite(LEDPIN, LOW);
  delay(100);
  digitalWrite(LEDPIN, HIGH);
  delay(100);
}

// --------------------------------------------------------------------------

// Function specifically used for loading the file header block
// into the buffer. Also sets inter_block_delay according to the file
// mode, and print some infos on the serial port.
int16_t load_header_block(byte *buf) {
  int16_t i, nbrd, blocklen;
  byte prevw, curw;
  byte btype, blen;
  byte type, mode;
  byte tmp;
  char name[9], ext[4];

  i = 0;

  nbrd = fd.read(&curw, 1);
  if(nbrd != 1) return -1;
  buf[i++] = curw;
  do {
    prevw = curw;
    nbrd = fd.read(&curw, 1);
    if(nbrd != 1) return -1;
    buf[i++] = curw;
  } while(prevw != 0x3c || curw != 0x5a);

  nbrd = fd.read(&btype, 1);
  if(nbrd != 1) return -1;
  buf[i++] = btype;

  nbrd = fd.read(&blen, 1);
  if(nbrd != 1) return -1;
  buf[i++] = blen;

  nbrd = fd.read(buf+i, 8);
  if(nbrd != 8) return -1;
  memcpy(name, buf+i, 8);
  name[8] = '\0';
  i += 8;

  nbrd = fd.read(buf+i, 3);
  if(nbrd != 3) return -1;
  memcpy(ext, buf+i, 3);
  ext[3] = '\0';
  i += 3;

  nbrd = fd.read(&type, 1);
  if(nbrd != 1) return -1;
  buf[i++] = type;

  nbrd = fd.read(&mode, 1);
  if(nbrd != 1) return -1;
  buf[i++] = mode;

  nbrd = fd.read(&tmp, 1);
  if(nbrd != 1) return -1;
  buf[i++] = tmp;

  nbrd = fd.read(&tmp, 1);
  if(nbrd != 1) return -1;
  buf[i++] = tmp;

  //print_block(buf, i);

  Serial.print(F("file type: "));
  switch(type) {
    case BASIC_FILE:
      Serial.println(F("basic file"));
      break;
    case DATA_FILE:
      Serial.println(F("data file"));
      break;
    case BINARY_FILE:
      Serial.println(F("binary file"));
      break;
    default:
      Serial.println(type);
  }

  Serial.print("file mode: ");
  switch(mode) {
    case BINARY_MODE:
      Serial.println(F("binary"));
      inter_block_delay = BINARY_MODE_DELAY;
      break;
    case TEXT_MODE:
      Serial.println(F("text"));
      inter_block_delay = TEXT_MODE_DELAY;
      break;
    default:
      Serial.println(mode);
      inter_block_delay = BINARY_MODE_DELAY;
  }

  Serial.print("inter block delay: ");
  Serial.print(inter_block_delay);
  Serial.println(" ms");

  return i;
}

// Function loading a block from the SD card.
int16_t load_block(byte *buf) {
  int16_t i, nbrd, blocklen;
  byte prevw, curw;
  byte btype, blen;

  i = 0;

  nbrd = fd.read(&curw, 1);
  if(nbrd != 1) return -1;
  buf[i++] = curw;
  do {
    prevw = curw;
    nbrd = fd.read(&curw, 1);
    if(nbrd != 1) return -1;
    buf[i++] = curw;
  } while(prevw != 0x3c || curw != 0x5a);

  nbrd = fd.read(&btype, 1);
  if(nbrd != 1) return -1;
  buf[i++] = btype;

  nbrd = fd.read(&blen, 1);
  if(nbrd != 1) return -1;
  buf[i++] = blen;

  if(blen == 0x00) blocklen = 256;
  else blocklen = blen;

  nbrd = fd.read(buf+i, blocklen-1);
  if(nbrd != blocklen-1) return -1;

  return i+blocklen-1;
}

bool is_end_block(byte *buf) {
  byte eblock[3] = {0xFF, 0x02, 0x00};
  uint8_t i = 0;
  while(buf[i] != 0x01) i++;
  while(buf[i] == 0x01) i++;
  if(buf[i++] != 0x3c) failure(F("invalid block"));
  if(buf[i++] != 0x5a) failure(F("invalid block"));
  if(buf[i++] != 0xFF) return false;
  if(buf[i++] != 0x02) return false;
  if(buf[i++] != 0x00) return false;
  return true;
}

// First version of the load_block function
int16_t load_block_(byte *buf) {
  int16_t i, nbrd, blocklen;
  byte btype, blen;

  nbrd = fd.read(buf, 20);
  if(nbrd < 20) return -1;
  for(i = 0; i < 16; i++)
    if(buf[i] != 0x01) return -1;
  if(buf[16]!=0x3c || buf[17]!=0x5a) return -1;

  btype = buf[18];
  blen = buf[19];

  if(blen == 0x00) blocklen = 256;
  else blocklen = blen;

  nbrd = fd.read(buf+20, blocklen-1);
  if(nbrd != blocklen-1) return -1;

  return blocklen+19;
}
