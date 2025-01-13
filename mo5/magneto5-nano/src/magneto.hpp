#ifndef __MAGNETO__
#define __MAGNETO__

#include <Arduino.h>
#include "tape.hpp"

#define MAGNETO_MOTOPIN PB3
#define MAGNETO_DATAOUT PB4

// Shortest delay in microseconds between two
// state changes of the DATOUT pin
#define MAGNETO_TOGGLE_DELAY 316

// Delay beetween each block of a file;
// 160 ms seems fine for binary files
// and tokenized basic files, but this
// must be increased to at least 1500 ms
// for ASCII text basic files (to allow
// each line to be proceeded).
#define MAGNETO_BINARY_MODE_DELAY 160 // ms
#define MAGNETO_TEXT_MODE_DELAY  1500 // ms

#define MAGNETO_BUFLEN 320

class magneto {
public:
  byte buffer[MAGNETO_BUFLEN];
  byte state; // Current state (0 or 1) of the DATOUT pin.
  uint16_t inter_block_delay; // Delay in ms between two blocks
  tape k7;

  magneto(tape &k7in) {
	k7 = k7in;
	k7.rewind();
    pinMode(MAGNETO_DATAOUT, OUTPUT);
    pinMode(MAGNETO_MOTOPIN, INPUT);
    inter_block_delay = MAGNETO_BINARY_MODE_DELAY;
    state = 0x00;
  }

  void failure(const __FlashStringHelper* msg);

  int8_t pausems(uint16_t dt);

  void reset_datout(void);

  void toggle_datout(void);

  void read_block(uint16_t len);
  
  // Search for the next block: if header is true,
  // search for the next header block, if header is false
  // search either for file block or an end block.
  // Returns true when such a block is found, false
  // otherwise. When a block is found, the byte pointed
  // to by btype is set to the block type, and the next
  // byte to be read is the one following btype.
  // block types: 0x00 for a header block,
  //              0x01 for a file block,
  //              0xFF for a end block.
  bool search_next_block(byte *btype, bool header);
  
  // Function specifically used for loading the file header block
  // into the buffer. Also sets inter_block_delay according to the file
  // mode, and print some infos on the serial port.
  int16_t load_next_header_block(void);
  
  int16_t load_next_block(bool *is_endblk);
  
  void read_k7();
};

#endif // __MAGNETO__