#ifndef NODE_CONFIGURATION_MANAGER_H
#define NODE_CONFIGURATION_MANAGER_H

#include <RF24.h>

class NodeConfigurationManager
{
    public:
        NodeConfigurationManager(RF24& radio, uint64_t address)
            : radio(radio), gatewayAddress(address) {}
        void handleNodeConfiguration(const char*);
    private:
        RF24 radio;
        uint64_t gatewayAddress;
};

#endif /* NODE_CONFIGURATION_MANAGER_H */