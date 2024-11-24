#include "serial_packets.hpp"

int recv_packet(byte *buf) {
    uint16_t len;
    while(!Serial.available());
    // first read the size of the packet
    Serial.readBytes((byte*)&len, sizeof(uint16_t));
    // next read the data forming the packet
    Serial.readBytes(buf, len);
    return len;
}

void send_packet(byte *buf, uint16_t len) {
    while(!Serial.availableForWrite());
    // first, send the size of the packet
    Serial.write((byte*)&len, sizeof(uint16_t));
    // next send the data forming the packet
    Serial.write(buf, len);
}

void send_ready(void) {
    send_packet((byte*)"READY", 5);
}
  
bool wait_ready(byte *buf) {
    uint16_t len = recv_packet(buf);
    if((len != 5) || (strncmp((const char*)buf, "READY", 5) != 0))
      return false;
    else
      return true;
}

void send_ack(void) {
    send_packet((byte*)"ACK", 3);
}

void send_nack(void) {
    send_packet((byte*)"NACK", 3);
}
  
bool wait_ack(byte *buf) {
    uint16_t len = recv_packet(buf);
    if((len != 3) || (strncmp((const char*)buf, "ACK", 3) != 0))
      return false;
    else
      return true;
}
