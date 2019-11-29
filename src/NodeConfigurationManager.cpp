#include "../include/NodeConfigurationManager.h"
#include "Packet.h"

#include <ArduinoLog.h>
#include <stdint.h>
#include <errno.h>

bool parsePayload(const char* payload, uint8_t* id, uint64_t* address)
{
    bool firstNumber = true;

    char buffer[64] = {0};
    snprintf(buffer, 64, "%s", payload);
    char* token = strtok(buffer, ",");
    while (token != NULL)
    {
        errno = 0;
        char *endptr = NULL;
        if(firstNumber)
        {
            *id = strtoul(token, &endptr, 16);
            firstNumber = false;
        }
        else
        {
            *address = strtoull(token, &endptr, 16);
        }
        
        if(!(errno == 0 && token && !*endptr))
        {
            Log.error(F("Conversion of '%s' failed. Error %d"), token, errno);
            return false;
        }
        
        token = strtok(NULL, ",");
    }
    return true;
}

void NodeConfigurationManager::handleNodeConfiguration(const char* payload)
{
    uint8_t id;
    uint64_t address;
    
    Log.notice(F("Decoding payload '%s'"), payload);
    
    if(parsePayload(payload, &id, &address))
    {
        radio.stopListening();
        radio.openWritingPipe(address);
        radio.setRetries(3, 5);
        
        packet.id = id;
        snprintf(packet.payload, sizeof(packet.payload), "%016llx", gatewayAddress);
        Log.notice(F("Sending Packet(id=%d,payload='%s')"), id, packet.payload);
        
        if(!radio.write(&packet, sizeof(packet)))
        {
            Log.warning(F("Did not recieve ack"));
        }
        
        radio.startListening();
    }
    else
    {
        Log.error(F("Could not decode payload"));
    }
}