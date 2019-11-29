#include <Arduino.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include <WiFiManager.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "../include/network/MQTTClient.h"
#include "../include/ConfigurationManager.h"
#include "../include/network/NetworkManager.h"
#include "../include/Packet.h"
#include "../include/NodeConfigurationManager.h"

#define BAUD_RATE 9600
#define CONFIG_FILENAME "mqtt_config"
#define MQTT_CLIENT_ID "weather_gateway"
#define RADIO_CS_PIN 5
#define RADIO_CE_PIN 4
#define RADIO_ADDRESS 0xABCDABCD71LL
#define RADIO_READING_PIPE 1

Packet packet = Packet();
ConfigurationManager configurationManager(CONFIG_FILENAME);
NetworkManager networkManager;
MQTTClient mqttClient(networkManager.getNetworkClient(), configurationManager.getConfiguration(), MQTT_CLIENT_ID);
RF24 radio(RADIO_CS_PIN, RADIO_CE_PIN);
NodeConfigurationManager nodeConfigurationManager(radio, RADIO_ADDRESS);

void setup()
{
  Serial.begin(BAUD_RATE);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.setSuffix([](Print* _logOutput){_logOutput->print('\n');});

  configurationManager.setup();
  networkManager.setup();
  mqttClient.setup();
  mqttClient.subscribe("gateway/configuration", [&](const char* payload) {
    Log.trace(F("Configuration request recieved"));
    nodeConfigurationManager.handleNodeConfiguration(payload);
  });

  if(radio.begin())
  {
    Log.notice(F("Setting radio configuration"));
    radio.setPALevel(RF24_PA_MIN);
    radio.setDataRate(RF24_250KBPS);
    radio.openReadingPipe(RADIO_READING_PIPE, RADIO_ADDRESS);
    radio.startListening();
  }
  else
  {
    Log.error("F(Radio configuration failed)");
  }
}

void loop()
{
  configurationManager.loop();
  networkManager.loop();
  mqttClient.loop();
  
  if(radio.available())
  {
    radio.read(&packet, sizeof(packet));
    Log.trace(F("Recieved Packet(id=%d,payload='%s')"), packet.id, packet.payload);
    char topicName[16] = {0};
    sprintf(topicName, "sensors/%d", packet.id);
    mqttClient.publish(topicName, packet.payload, true);
    memset(packet.payload, 0, sizeof(packet.payload));
  }
  
  if(mqttClient.cannotConnect())
  {
    configurationManager.requestConfiguration();
  }
}