#ifndef RADIO_H
#define RADIO_H
#include <RF24.h>
#include "Runnable.h"

class RadioReciever: public Runnable
{
    public:
        RadioReciever(int cs, int ce, uint8_t paLevel, uint8_t dataRate, uint8_t pipe, uint64_t address): 
            radio(RF24(cs, ce)), paLevel(paLevel), dataRate(dataRate), pipe(pipe), address(address) {}

        void setup() override;
        void loop() override;

        struct Packet {
            uint8_t address;
            char payload[31];
        } packet;

        void setPacketHandler(void (*packetHandler)(const RadioReciever::Packet&))
        {
            this->packetHandler = packetHandler;
        }
    private:
        RF24 radio;
        const uint8_t paLevel;
        const uint8_t dataRate;
        const uint8_t pipe;
        const uint64_t address;
        void (*packetHandler)(const RadioReciever::Packet&);
};

#endif /* RADIO_H */