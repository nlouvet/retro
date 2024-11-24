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

void print_root(void);
int16_t load_next_header_block(byte *buf);
int16_t load_next_block(byte *buf, bool *is_endblk);

void reset_datout() {
  state = 0x01;
  digitalWrite(DATAOUT, state);
}

void toggle_datout() {
  state ^= 0x01;
  digitalWrite(DATAOUT, state);

}

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
    if(millis() - tstamp > dt) return -1;
    else while(millis() - tstamp <= dt);
  }
  return 1;
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

  print_root();
  Serial.setTimeout(60000); // waiting for the user to send a file name...
  bool cont = 1;
  do {
    Serial.print(F("[enter filename] "));
    String fname = Serial.readStringUntil('\n');
    fname.trim();
    Serial.print(F("Trying to open "));
    Serial.println(fname.c_str());
    fd = SD.open(fname.c_str(), FILE_READ);
    if(!fd) Serial.println("Unable to open the file...");
    else Serial.println(F("File opened!"));
  } while(!fd);

  reset_datout();

  int16_t nbrd = 0;
  uint16_t i = 0;
  bool is_endblk = false;

  do {

    Serial.print(F("Trying to load header block "));
    Serial.println(i);
    nbrd = load_next_header_block(buffer);
    if(nbrd == -1) failure(F("error while loading a header block"));
    while(digitalRead(MOTOPIN) == HIGH);
    Serial.println(F("Reading file..."));
    delay(2500);
    read_block(buffer, nbrd);
    delay(2500);

    pausems(0);
    do {
      Serial.print(F("Loading block "));
      Serial.println(i);
      nbrd = load_next_block(buffer, &is_endblk);
      if(nbrd == -1) failure(F("error while loading a block"));
      if(pausems(inter_block_delay) == -1) failure(F("error delay exceeded"));
      read_block(buffer, nbrd);
      i++;
      pausems(0);
    } while(!is_endblk);

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

// Search in the file the next block: if header is true,
// search for the next header block, if header is false
// search either for file block or an end block.
// Returns true when such a block is found, false
// otherwise. When a block is found, the byte pointed
// to by btype is set to the block type, and the next
// byte to be read is the one following btype.
// block types: 0x00 for a header block,
//              0x01 for a file block,
//              0xFF for a end block.
bool search_next_block(byte *btype, bool header) {
  bool stop = false;
  uint16_t nbrd;
  byte w;

  enum {st_init, st_01, st_3c, st_5a} st = st_init;
  uint8_t nb_01 = 0;

  while(!stop) {
    if(fd.available()) {
      nbrd = fd.read(&w, 1);
      if(nbrd != 1) return false;
      switch(st) {
        case st_init:
          if(w == 0x01) { nb_01 = 1; st = st_01; }
          break;
        case st_01:
          if(w == 0x01) { nb_01++; }
          else {
            if((w == 0x3c) && (nb_01 >= 8)) { nb_01 = 0; st = st_3c; }
            else { nb_01 = 0; st = st_init; }
          }
          break;
        case st_3c:
          if(w == 0x01) { nb_01 = 1; st = st_01; }
          else {
            nb_01 = 0;
            if(w == 0x5a) st = st_5a;
            else st = st_init;
          }
          break;
        case st_5a:
          if(header) {
            if(w == 0x01) { nb_01 = 1; st = st_01; }
            else {
              nb_01 = 0;
              if(w == 0x00) stop = true;
              else st = st_init;  
            }
          }
          else {
            nb_01 = 0;
            if((w == 0x01) || (w == 0xFF)) stop = true;
            else st = st_init;
          }
          break;
      }
    }
    else {
      // no more chance to find a valid block
      return false;
    }
  }
  *btype = w;
  return true;
}

// Function specifically used for loading the file header block
// into the buffer. Also sets inter_block_delay according to the file
// mode, and print some infos on the serial port.
int16_t load_next_header_block(byte *buf) {
  uint16_t i, nbrd, blocklen;
  //state_t st = st_init;
  bool stop = false;
  byte w, tmp;
  byte btype, blen;
  byte type, mode;
  char name[9], ext[4];

  if(!search_next_block(&btype, true)) return -1;
  Serial.println(btype);
  if(btype != 0x00) return -1;
  // adding the begining of a header block in the buffer
  for(i = 0; i < 16; i++) buf[i] = 0x01;
  buf[i++] = 0x3c;
  buf[i++] = 0x5a;
  buf[i++] = 0x00;
  // read and store the length of the block
  nbrd = fd.read(&blen, 1);
  if(nbrd != 1) return -1;
  buf[i++] = blen;
  // read and store the name of the file
  nbrd = fd.read(buf+i, 8);
  if(nbrd != 8) return -1;
  memcpy(name, buf+i, 8);
  name[8] = '\0';
  i += 8;
  // read and store the extension of the file
  nbrd = fd.read(buf+i, 3);
  if(nbrd != 3) return -1;
  memcpy(ext, buf+i, 3);
  ext[3] = '\0';
  i += 3;
  // read and store the type of the file
  nbrd = fd.read(&type, 1);
  if(nbrd != 1) return -1;
  buf[i++] = type;
  // read and store the mode of the file
  nbrd = fd.read(&mode, 1);
  if(nbrd != 1) return -1;
  buf[i++] = mode;
  // should be the same as the previous byte
  nbrd = fd.read(&tmp, 1);
  if(nbrd != 1) return -1;
  buf[i++] = tmp;
  // read and store the checksum of the block
  nbrd = fd.read(&tmp, 1);
  if(nbrd != 1) return -1;
  buf[i++] = tmp;

  Serial.print(F("type: "));
  switch(type) {
    case BASIC_FILE:  Serial.println(F("basic"));  break;
    case DATA_FILE:   Serial.println(F("data"));   break;
    case BINARY_FILE: Serial.println(F("binary")); break;
    default: Serial.println(type);
  }
  Serial.print(F("mode: "));
  switch(mode) {
    case BINARY_MODE: Serial.println(F("binary")); inter_block_delay = BINARY_MODE_DELAY; break;
    case TEXT_MODE:   Serial.println(F("text"));   inter_block_delay = TEXT_MODE_DELAY;   break;
    default:          Serial.println(mode);        inter_block_delay = BINARY_MODE_DELAY;
  }
  Serial.print(F("inter block delay: "));
  Serial.print(inter_block_delay);
  Serial.println(F(" ms"));

  return i;
}

// Function loading a block from the SD card.
int16_t load_next_block(byte *buf, bool *is_endblk) {
  int16_t i, nbrd, blocklen;
  byte btype, blen, bchk;
  *is_endblk = false;

  if(!search_next_block(&btype, false)) return -1;
  if((btype != 0x01) && (btype != 0xFF)) return -1;

  if(btype == 0x01) { // file block
    *is_endblk = false;
    // adding the begining of a header block in the buffer
    for(i = 0; i < 16; i++) buf[i] = 0x01;
    buf[i++] = 0x3c;
    buf[i++] = 0x5a;
    buf[i++] = 0x01;
    // length of the block
    nbrd = fd.read(&blen, 1);
    if(nbrd != 1) return -1;
    buf[i++] = blen;
    if(blen == 0x00) blocklen = 256;
    else blocklen = blen;
    // content of the block with checksum
    nbrd = fd.read(buf+i, blocklen-1);
    if(nbrd != blocklen-1) return -1;
    // return the total length of the buffer
    return i+blocklen-1;
  }
  else if(btype == 0xFF) { // end block
    *is_endblk = true;
    // two more bytes to read, that should be
    // 0x02 (length) and 0x00 (checksum)
    // not to check their values
    nbrd = fd.read(&blen, 1); // should be 0x02
    if(nbrd != 1) return -1;
    nbrd = fd.read(&bchk, 1); // should be 0x00
    if(nbrd != 1) return -1;
    // store an end block into the buffer
    for(i = 0; i < 16; i++) buf[i] = 0x01;
    buf[i++] = 0x3c;
    buf[i++] = 0x5a;
    buf[i++] = 0xFF;
    buf[i++] = blen;
    buf[i++] = bchk;
    return 21;
  }
  else return -1; // file error
}


void print_root(void) {
  Serial.println(F("Listing available files:"));
  File root = SD.open("/");
  while (true) {
    File entry =  root.openNextFile();
    if (! entry) break; // no more files
    Serial.print(entry.name());
    if(entry.isDirectory()) Serial.println("/");
    else {
      // files have sizes, directories do not
      Serial.print(" ");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
  root.close();
  Serial.println(F("done!"));
}










/*
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
*/

/*
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
*/

/*
// Function specifically used for loading the file header block
// into the buffer. Also sets inter_block_delay according to the file
// mode, and print some infos on the serial port.
int16_t load_next_header_block_(byte *buf) {
  uint16_t i, nbrd, blocklen;
  //state_t st = st_init;
  bool stop = false;
  byte w, tmp;
  byte btype, blen;
  byte type, mode;
  char name[9], ext[4];

  enum {st_init, st_01, st_3c, st_5a} st = st_init;
  uint8_t nb_01 = 0;

  while(!stop) {
    if(fd.available()) {
      nbrd = fd.read(&w, 1);
      if(nbrd != 1) return -1;
      switch(st) {
        case st_init:
          if(w == 0x01) { nb_01 = 1; st = st_01; }
          break;
        case st_01:
          if(w == 0x01) { nb_01++; }
          else {
            if((w == 0x3c) && (nb_01 >= 8)) { nb_01 = 0; st = st_3c; }
            else { nb_01 = 0; st = st_init; }
          }
          break;
        case st_3c:
          if(w == 0x01) { nb_01 = 1; st = st_01; }
          else {
            nb_01 = 0;
            if(w == 0x5a) st = st_5a;
            else st = st_init;
          }
          break;
        case st_5a:
          if(w == 0x01) { nb_01 = 1; st = st_01; }
          else {
            nb_01 = 0;
            if(w == 0x00) stop = true;
            else st = st_init;
          }
          break;
      }
    }
    else {
      // no more chance to find a header block
      return -1;
    }
  }

  // adding the begining of a header block in the buffer
  for(i = 0; i < 16; i++) buf[i] = 0x01;
  buf[i++] = 0x3c;
  buf[i++] = 0x5a;
  buf[i++] = 0x00;
  // read and store the length of the block
  nbrd = fd.read(&blen, 1);
  if(nbrd != 1) return -1;
  buf[i++] = blen;
  // read and store the name of the file
  nbrd = fd.read(buf+i, 8);
  if(nbrd != 8) return -1;
  memcpy(name, buf+i, 8);
  name[8] = '\0';
  i += 8;
  // read and store the extension of the file
  nbrd = fd.read(buf+i, 3);
  if(nbrd != 3) return -1;
  memcpy(ext, buf+i, 3);
  ext[3] = '\0';
  i += 3;
  // read and store the type of the file
  nbrd = fd.read(&type, 1);
  if(nbrd != 1) return -1;
  buf[i++] = type;
  // read and store the mode of the file
  nbrd = fd.read(&mode, 1);
  if(nbrd != 1) return -1;
  buf[i++] = mode;
  // should be the same as the previous byte
  nbrd = fd.read(&tmp, 1);
  if(nbrd != 1) return -1;
  buf[i++] = tmp;
  // read and store the checksum of the block
  nbrd = fd.read(&tmp, 1);
  if(nbrd != 1) return -1;
  buf[i++] = tmp;

  Serial.print(F("type: "));
  switch(type) {
    case BASIC_FILE:
      Serial.println(F("basic"));
      break;
    case DATA_FILE:
      Serial.println(F("data"));
      break;
    case BINARY_FILE:
      Serial.println(F("binary"));
      break;
    default:
      Serial.println(type);
  }

  Serial.print(F("mode: "));
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

  Serial.print(F("inter block delay: "));
  Serial.print(inter_block_delay);
  Serial.println(F(" ms"));

  return i;
}
*/