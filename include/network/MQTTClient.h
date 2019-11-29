#ifndef NETWORK_MQTT_H
#define NETWORK_MQTT_H
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "../Runnable.h"
#include "../Configuration.h"

#define MQTT_CONNECT_MAX_TRIES 5
#define MAX_SUBSCRIPTIONS 2
#define MAX_PAYLOAD_SIZE 64

class MQTTClient: public Runnable
{
    public:
        MQTTClient(Client& networkClient, const Configuration& configuration, const char* id) : 
            client(networkClient), configuration(configuration), id(id) {}
        void setup() override;
        void loop() override;
        void publish(const char*, const char*, bool);
        inline bool cannotConnect() const
        {
            return this->connectionRetries > 5;
        }
        void subscribe(char* topic, void (*callback)(const char*));
        void (*getTopicCallback(const char*))(const char*);
        
    private:
        void reconnect(bool);
        struct Subscription {
            bool set;
            char* topic;
            void (*callback)(const char*);
        };
        Subscription subscriptions[MAX_SUBSCRIPTIONS];
        PubSubClient client;
        const Configuration& configuration;
        const char* id;
        unsigned long lastTryMs;
        int connectionRetries;
};

#endif /* NETWORK_MQTT_H */