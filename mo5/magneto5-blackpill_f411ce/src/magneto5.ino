#include <Arduino.h>

#include "serpackets.hpp"
#include "magneto.hpp"
#include "SPIFlash.h"

void failure(const __FlashStringHelper* msg) {
	Serial.print(F("FAILURE: "));
	Serial.println(msg);
	while(1);
}

void setup() {
	Serial.begin(115200);
	while (!Serial);

	Serial.println("Initializing magneto5...");

	K7 k7(64); // 64 kibytes reserved

	/*
	SPIFlash flash(PA4);
	flash.begin();
	Serial.println(flash.getUniqueID());
	Serial.println(flash.getCapacity());
	
	if(!flash.eraseChip())
    	Serial.println(F("Error erasing chip"));
	byte tab[5] = {0x01, 0x03, 0x07, 0x06, 0x08};
	if(!flash.writeByteArray(0, tab, 5, true))
		Serial.println(F("Error writing to flash"));
	
	byte tab[5];
	if(!flash.readByteArray(0, tab, 5, true))
		Serial.println(F("Error reading flash"));
	for(int16_t i = 0; i < 5; i++)
		Serial.println(tab[i], HEX);
	*/
	Serial.println(F("READY"));

	byte buffer[SERPACKETS_MAX_SIZE];
	int32_t num_exp = 0;
	int32_t num_rcv = 0;
	int16_t s;

	send_ready();
	num_exp = wait_prelude();
	while((s = wait_data(buffer)) > 0) {
		k7.add(buffer, s);
		num_rcv += s;
		send_ack();
	}

	if(s == -1 || num_rcv != num_exp) {
		Serial.print(F("return value: "));
		Serial.println(s);
		Serial.print(F("number of bytes expected: "));
		Serial.println((int)num_exp);
		Serial.print(F("number of bytes received: "));
		Serial.println((int)num_rcv);
		failure(F("error while loading the k7..."));
	}

	magneto M(k7);
	M.read_k7();

	Serial.println(F("END"));
}

void loop() {
}
