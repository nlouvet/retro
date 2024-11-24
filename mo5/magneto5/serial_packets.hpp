#include <Arduino.h>

int recv_packet(byte *buf);
void send_packet(byte *buf, uint16_t len);
void send_ready(void);
bool wait_ready(byte *buf);
void send_ack(void);
void send_nack(void);
bool wait_ack(byte *buf);
