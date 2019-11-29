#include <ArduinoLog.h>
#include "../../include/network/MQTTClient.h"
#include "../../include/Configuration.h"

void MQTTClient::setup()
{
    this->client.setServer(this->configuration.getMqttHost(), this->configuration.getMqttPort());
    connectionRetries = 0;
    reconnect(true);
    client.setCallback([this](char* topic, byte* payload, unsigned int length) {
        Log.trace(F("MQTT message recieved"));
        auto callback = this->getTopicCallback(topic);
        if(callback)
        {
            char payloadString[MAX_PAYLOAD_SIZE] = {0};
            for(int i = 0; i < MAX_PAYLOAD_SIZE - 1; i++)
            {
                payloadString[i] = (char)payload[i];
            }
            
            if(length >= MAX_PAYLOAD_SIZE)
            {
                payloadString[MAX_PAYLOAD_SIZE - 1] = '\0';
            }
            else
            {
                payloadString[length] = '\0';
            }

            callback(payloadString);
            payloadString[0] = '\0';
        }
    });
}

void MQTTClient::loop()
{
    this->reconnect(false);
    this->client.loop();
}

void MQTTClient::publish(const char* topic, const char* payload, boolean retained = false)
{
    Log.trace(F("Publishing \"%s\" to topic %s"), payload, topic);
    client.publish(topic, payload, retained);
}

void MQTTClient::subscribe(char* topic, void (*callback)(const char*))
{
    for(int i = 0; i < MAX_SUBSCRIPTIONS; i++)
    {
        if(!subscriptions[i].set)
        {
            subscriptions[i].set = true;
            subscriptions[i].callback = callback;
            subscriptions[i].topic = topic;
            client.subscribe(topic);
        }
    }
}

 void (*MQTTClient::getTopicCallback(const char* topic))(const char*)
{
    Log.trace(F("Looking for callback"));
    for(int i = 0; i < MAX_SUBSCRIPTIONS; i++)
    {
        if(strcmp(subscriptions[i].topic, topic) == 0 && subscriptions[i].set)
        {
            Log.trace(F("Found topic callback"));
            return subscriptions[i].callback;
        }
    }
    return nullptr;
}

void MQTTClient::reconnect(bool first)
{
    if(!this->client.connected())
    {
        if(lastTryMs + 5000 < millis() || first)
        {
            Log.notice(F("Connecting to MQTT broker %s with username %s"), 
                this->configuration.getMqttHost(), this->configuration.getMqttUsername());
            const char* username = this->configuration.getMqttUsername();
            const char* password = this->configuration.getMqttPassword();
            if(strlen(username) > 0)
            {
                this->client.connect(this->id, username, password);
            }
            else
            {
                this->client.connect(this->id);
            }

            if(client.connected())
            {
                this->connectionRetries = 0;
            }
            else
            {
                this->connectionRetries++;
            }
            
            this->lastTryMs = millis();
        }
    }
}