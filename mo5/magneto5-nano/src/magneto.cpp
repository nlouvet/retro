#include "magneto.hpp"

// ***************************************************************************
// magneto class functions definitions
// ***************************************************************************

// file type: 00=Basic 01=Data 02=Binaire
#define BASIC_FILE  0x00
#define DATA_FILE   0x01
#define BINARY_FILE 0x02
// file mode: 00=Binaire FF=Texte
#define BINARY_MODE 0x00
#define TEXT_MODE   0xFF

void magneto::failure(const __FlashStringHelper* msg) {
  Serial.print(F("MAGNETO FAILURE: "));
  Serial.println(msg);
  while(1);
}

int8_t magneto::pausems(uint16_t dt) {
  static uint32_t tstamp = 0;
  if(dt == 0) tstamp = millis();
  else {
    if(millis() - tstamp > dt) return -1;
    else while(millis() - tstamp <= dt);
  }
  return 1;
}

void magneto::reset_datout(void) {
  state = 0x01;
  digitalWrite(MAGNETO_DATAOUT, state);
}

void magneto::toggle_datout(void) {
  state ^= 0x01;
  digitalWrite(MAGNETO_DATAOUT, state);

}

void magneto::read_block(uint16_t len) {
  for(uint16_t i = 0; i < len; i++) {
    byte w = buffer[i];
    for(uint8_t j = 0; j < 8; j++) {
      toggle_datout();
      delayMicroseconds(MAGNETO_TOGGLE_DELAY);
      if(w & 0x80) toggle_datout();
      delayMicroseconds(MAGNETO_TOGGLE_DELAY);
      w <<= 1;
    }
  }
  reset_datout();
}

bool magneto::search_next_block(byte *btype, bool header) {
  bool stop = false;
  uint16_t nbrd;
  byte w;

  enum {st_init, st_01, st_3c, st_5a} st = st_init;
  uint8_t nb_01 = 0;

  while(!stop) {
    if(k7.available()) {
      nbrd = k7.read(&w, 1);
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

int16_t magneto::load_next_header_block(void) {
char name[9], ext[4];
  uint16_t i, nbrd;
  byte btype, blen;
  byte type, mode;
  byte w;

  if(!search_next_block(&btype, true)) return -1;
  if(btype != 0x00) return -1;
  // adding the begining of a header block in the buffer
  for(i = 0; i < 16; i++) buffer[i] = 0x01;
  buffer[i++] = 0x3c;
  buffer[i++] = 0x5a;
  buffer[i++] = 0x00;
  // read and store the length of the block
  nbrd = k7.read(&blen, 1);
  if(nbrd != 1) return -1;
  buffer[i++] = blen;
  // read and store the name of the file
  nbrd = k7.read(buffer+i, 8);
  if(nbrd != 8) return -1;
  memcpy(name, buffer+i, 8);
  name[8] = '\0';
  i += 8;
  // read and store the extension of the file
  nbrd = k7.read(buffer+i, 3);
  if(nbrd != 3) return -1;
  memcpy(ext, buffer+i, 3);
  ext[3] = '\0';
  i += 3;
  // read and store the type of the file
  nbrd = k7.read(&type, 1);
  if(nbrd != 1) return -1;
  buffer[i++] = type;
  // read and store the mode of the file
  nbrd = k7.read(&mode, 1);
  if(nbrd != 1) return -1;
  buffer[i++] = mode;
  // should be the same as the previous byte
  nbrd = k7.read(&w, 1);
  if(nbrd != 1) return -1;
  buffer[i++] = w;
  // read and store the checksum of the block
  nbrd = k7.read(&w, 1);
  if(nbrd != 1) return -1;
  buffer[i++] = w;

  Serial.print(F("file type: "));
  switch(type) {
    case BASIC_FILE:  Serial.println(F("basic"));  break;
    case DATA_FILE:   Serial.println(F("data"));   break;
    case BINARY_FILE: Serial.println(F("binary")); break;
    default: Serial.println(type);
  }
  Serial.print(F("file mode: "));
  switch(mode) {
    case BINARY_MODE: Serial.println(F("binary")); inter_block_delay = MAGNETO_BINARY_MODE_DELAY; break;
    case TEXT_MODE:   Serial.println(F("text"));   inter_block_delay = MAGNETO_TEXT_MODE_DELAY;   break;
    default:          Serial.println(mode);        inter_block_delay = MAGNETO_BINARY_MODE_DELAY;
  }
  Serial.print(F("inter block delay: "));
  Serial.print(inter_block_delay);
  Serial.println(F(" ms"));

  return i;
}

// Function loading a block from the SD card.
int16_t magneto::load_next_block(bool *is_endblk) {
  int16_t i, nbrd, blocklen;
  byte btype, blen, bchk;
  *is_endblk = false;

  if(!search_next_block(&btype, false)) return -1;
  if((btype != 0x01) && (btype != 0xFF)) return -1;

  if(btype == 0x01) { // file block
    *is_endblk = false;
    // adding the begining of a header block in the buffer
    for(i = 0; i < 16; i++) buffer[i] = 0x01;
    buffer[i++] = 0x3c;
    buffer[i++] = 0x5a;
    buffer[i++] = 0x01;
    // length of the block
    nbrd = k7.read(&blen, 1);
    if(nbrd != 1) return -1;
    buffer[i++] = blen;
    if(blen == 0x00) blocklen = 256;
    else blocklen = blen;
    // content of the block with checksum
    nbrd = k7.read(buffer+i, blocklen-1);
    if(nbrd != blocklen-1) return -1;
    // return the total length of the buffer
    return i+blocklen-1;
  }
  else if(btype == 0xFF) { // end block
    *is_endblk = true;
    // two more bytes to read, that should be
    // 0x02 (length) and 0x00 (checksum)
    // not to check their values
    nbrd = k7.read(&blen, 1); // should be 0x02
    if(nbrd != 1) return -1;
    nbrd = k7.read(&bchk, 1); // should be 0x00
    if(nbrd != 1) return -1;
    // store an end block into the buffer
    for(i = 0; i < 16; i++) buffer[i] = 0x01;
    buffer[i++] = 0x3c;
    buffer[i++] = 0x5a;
    buffer[i++] = 0xFF;
    buffer[i++] = blen;
    buffer[i++] = bchk;
    return 21;
  }
  else return -1; // file error
}

void magneto::read_k7(void) {
  reset_datout();

  int16_t nbrd = 0;
  uint16_t i = 0;
  bool is_endblk = false;

  do {

    Serial.print(F("Trying to load header block "));
    Serial.println(i);
    nbrd = load_next_header_block();
    if(nbrd == -1) failure(F("error while loading a header block"));
    while(digitalRead(MAGNETO_MOTOPIN) == HIGH);
    Serial.println(F("Reading file..."));
    delay(2500);
    read_block(nbrd);
    delay(2500);

    pausems(0);
    do {
      Serial.print(F("Loading block "));
      Serial.println(i);
      nbrd = load_next_block(&is_endblk);
      if(nbrd == -1) failure(F("error while loading a block"));
      if(pausems(inter_block_delay) == -1) failure(F("error delay exceeded"));
      read_block(nbrd);
      i++;
      pausems(0);
    } while(!is_endblk);

    Serial.println(F("Stop reading file..."));
    delay(2500);

  } while(k7.available());

  Serial.println(F("End of the k7 reached..."));
}
