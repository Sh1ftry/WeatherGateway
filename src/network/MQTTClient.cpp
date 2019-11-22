#include <ArduinoLog.h>
#include "MQTTClient.h"
#include "../Configuration.h"

void MQTTClient::setup()
{
    this->client.setServer(this->configuration.getMqttHost(), this->configuration.getMqttPort());
    connectionRetries = 0;
    reconnect(true);
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