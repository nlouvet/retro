#include <Arduino.h>

#define SERPACKETS_MAX_SIZE 320

#define SERPACKETS_PRELUDE 0xFF
#define SERPACKETS_DATA    0xFE
#define SERPACKETS_END     0xFD
#define SERPACKETS_READY   0xFC
#define SERPACKETS_ACK     0xFB

bool send_prelude(uint32_t size32);
bool send_ready(void);
bool send_ack(void);
bool send_end(void);

int32_t wait_prelude(void);
bool wait_ready(void);
bool wait_ack(void);
int16_t wait_data(byte *buf);