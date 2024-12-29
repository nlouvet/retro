#include <Arduino.h>
#include <SdFat.h>

#include "serpackets.hpp"
#include "magneto.hpp"

void failure(const __FlashStringHelper* msg) {
	Serial.print(F("FAILURE: "));
	Serial.println(msg);
	while(1);
}

void success(const __FlashStringHelper* msg) {
	Serial.print(F("SUCCESS: "));
	Serial.println(msg);
}

void setup() {
	Serial.begin(115200);
	while (!Serial);

	Serial.println("Initializing magneto5...");

	SdFat sd;
	pinMode(SD_CARD_CS, OUTPUT); //Setup SD card chipselect pin
	if(!sd.begin(SD_CARD_CS, SPI_FULL_SPEED))
		failure(F("No SD card..."));
	success(F("SD card initialized!"));

	File file;
	file = sd.open("temporary.k7", O_RDWR | O_CREAT | O_TRUNC);
	if(!file)
		failure(F("Unable to open the file..."));
	success(F("Temporary file open!"));

	K7 k7(&file);

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
