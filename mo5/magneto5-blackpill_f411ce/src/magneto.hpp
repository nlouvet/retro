#include <Arduino.h>

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

class K7 {
private:
	byte *tape;
	uint32_t max_size; // maximum space available in bytes
	uint32_t cur_size; // current space occupied in bytes
	uint32_t position; // current position on the tape
	void failure(const __FlashStringHelper* msg);
	
public:
	// default constructor: absolutely empty K7
	K7();

	// K7 initialized to receive at most s kibytes 
	K7(uint32_t s);

	// actually free the memory allocated to the K7 tape
	~K7();

	// true if the K7 contains data to be read, false
	// otherwise
	bool available();

	// rewind the K7 for reading it afterwards
	void rewind();

	// when loading the K7, add data to it
	// returns the number of bytes actually
	// added to the K7
	int32_t add(const byte *src, uint32_t size);

	// read at most size bytes from the K7, and
	// store it in the buffer pointed to by dst
	// return the number of bytes read
	int32_t read(byte *dst, uint32_t size);
};

class magneto {
public:
  byte buffer[MAGNETO_BUFLEN];
  byte state; // Current state (0 or 1) of the DATOUT pin.
  uint16_t inter_block_delay; // Delay in ms between two blocks
  K7 k7;

  magneto(K7 &k7in) {
	k7 = k7in;
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
