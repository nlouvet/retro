#include "tape.hpp"

// ***************************************************************************
// tape class functions definitions
// ***************************************************************************

void tape::failure(const __FlashStringHelper* msg) {
  Serial.print(F("tape FAILURE: "));
  Serial.println(msg);
  while(1);
}

tape::tape() {
	file_ptr = NULL;
}

tape::tape(File *fptr) {
	file_ptr = fptr;
}

tape::~tape() {
	file_ptr = NULL;
}

bool tape::available() {
	return file_ptr->available() > 0; // ???
}

void tape::rewind() {
	file_ptr->rewind();
}

int32_t tape::write(const byte *src, uint32_t size) { // ???
	uint32_t nbwr = file_ptr->write(src, size);
	if(nbwr == 0) return -1;
	else return nbwr;
}

int32_t tape::read(byte *dst, uint32_t size) {
	return file_ptr->read(dst, size); // ???
	return 0;
}
