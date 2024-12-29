#include "serpackets.hpp"

bool send_prelude(uint32_t size32) {
	while(!Serial.availableForWrite());
	// type of the packet
	uint8_t type = SERPACKETS_PRELUDE;
	Serial.write((char*)&type, sizeof(uint8_t));
	// size of the file to be sent
	Serial.write((char*)&size32, sizeof(uint32_t));
	return true;
}

bool send_ready(void) {
	while(!Serial.availableForWrite());
	// type of the packet
	uint8_t type = SERPACKETS_READY;
	Serial.write((char*)&type, sizeof(uint8_t));
	return true;
}

bool send_ack(void) {
	while(!Serial.availableForWrite());
	// type of the packet
	uint8_t type = SERPACKETS_ACK;
	Serial.write((char*)&type, sizeof(uint8_t));
	return true;
}

bool send_end(void) {
	while(!Serial.availableForWrite());
	// type of the packet
	uint8_t type = SERPACKETS_END;
	Serial.write((char*)&type, sizeof(uint8_t));
	return true;
}

bool send_data(byte *buf, uint16_t size16) {
	while(!Serial.availableForWrite());
	// type of the packet
	uint8_t type = SERPACKETS_DATA;
	Serial.write((char*)&type, sizeof(uint8_t));
	// send the size of the packet
	if(size16 > SERPACKETS_MAX_SIZE) return false;
	Serial.write((char*)&size16, sizeof(uint16_t));
	// send the data forming the packet
	Serial.write(buf, size16);
	return true;
}



int32_t wait_prelude(void) {
	while(!Serial.available());
	// check the type of the packet received
	uint8_t recv_type;
	Serial.readBytes((char*)&recv_type, sizeof(uint8_t));
	if(recv_type != SERPACKETS_PRELUDE) return false;
	// read the size of the file to be received
	uint32_t size32;
	Serial.readBytes((char*)&size32, sizeof(uint32_t));
	return size32;
}

bool wait_ready(void) {
	while(!Serial.available());
	// check the type of the packet received
	uint8_t recv_type;
	Serial.readBytes((char*)&recv_type, sizeof(uint8_t));
	if(recv_type != SERPACKETS_READY) return false;
	return true;
}

bool wait_ack(void) {
	while(!Serial.available());
	// check the type of the packet received
	uint8_t recv_type;
	Serial.readBytes((char*)&recv_type, sizeof(uint8_t));
	if(recv_type != SERPACKETS_ACK) return false;
	return true;
}

int16_t wait_data(byte *buf) {
	while(!Serial.available());
	// check the type of the packet received
	uint8_t recv_type;
	Serial.readBytes((char*)&recv_type, sizeof(uint8_t));
	if(recv_type == SERPACKETS_END) return 0;
	if(recv_type != SERPACKETS_DATA) return -1;
	// read the size of the packet
	uint16_t size16;
	Serial.readBytes((char*)&size16, sizeof(uint16_t));
	if(size16 > SERPACKETS_MAX_SIZE) return -1;
	// next read the data forming the packet
	Serial.readBytes((char*)buf, size16);
	return size16;
}
