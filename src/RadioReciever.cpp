#include <ArduinoLog.h>
#include "RadioReciever.h"

void RadioReciever::setup()
{
    if(this->radio.begin())
    {
        Log.notice(F("Setting radio configuration"));
        this->radio.setPALevel(RF24_PA_MIN);
        this->radio.setDataRate(RF24_250KBPS);
        this->radio.openReadingPipe(1, this->address);
        this->radio.startListening();
    }
    else
    {
        Log.error(F("Reciever initialization failed"));
    }
}

void RadioReciever::loop()
{
    if(this->radio.available() && this->packetHandler)
    {
        radio.read(&this->packet, sizeof(this->packet));
        this->packetHandler(this->packet);
        memset(this->packet.payload, 0, sizeof(this->packet.payload));
    }
}