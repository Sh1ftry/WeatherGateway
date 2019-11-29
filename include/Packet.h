#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

extern struct Packet
{
  uint8_t id;
  char payload[31];
} packet;

#endif /* PACKET_H */