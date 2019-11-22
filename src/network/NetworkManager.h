#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <ESP8266WiFi.h>
#include "Configuration.h"
#include "../Runnable.h"

class NetworkManager: public Runnable
{
    public:
        void setup() override;
        void loop() override;

        inline Client& getNetworkClient()
        {
            return this->client;
        }
    private:
        unsigned long lastConnectionCheckMs;
        WiFiClient client;

        void connect();
};

#endif /* NETWORK_MANAGER_H */