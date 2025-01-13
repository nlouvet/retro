#ifndef __TAPE__
#define __TAPE__

#include <Arduino.h>
#include <SdFat.h>

class tape {
private:
	File *file_ptr;
	void failure(const __FlashStringHelper* msg);
	
public:
	// default constructor: absolutely empty tape
	tape();

	// tape initialized from a File*
	tape(File*);

	// actually free the memory allocated to the tape
	~tape();

	// true if the tape contains data to be read, false
	// otherwise
	bool available();

	// rewind the tape for reading it afterwards
	void rewind();

	// add data to the tape
	// returns the number of bytes actually
	// added to the tape
	int32_t write(const byte *src, uint32_t size);

	// read at most size bytes from the tape, and
	// store it in the buffer pointed to by dst
	// return the number of bytes read
	int32_t read(byte *dst, uint32_t size);
};

#endif // __TAPE__