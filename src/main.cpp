#include <Arduino.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include <WiFiManager.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "network/MQTTClient.h"
#include "ConfigurationManager.h"
#include "network/NetworkManager.h"
#include "RadioReciever.h"

const uint16_t BAUD_RATE=9600;

const char* CONFIG_FILENAME="mqtt_config";

const char* MQTT_CLIENT_ID="weather_gateway";

const uint8_t RADIO_CS=5;
const uint8_t RADIO_CE=4;
const uint64_t RADIO_ADDRESS=0xABCDABCD71LL;
const uint8_t RADIO_PIPE=1;

ConfigurationManager configurationManager(CONFIG_FILENAME);
NetworkManager networkManager;
MQTTClient mqttClient(networkManager.getNetworkClient(), configurationManager.getConfiguration(), MQTT_CLIENT_ID);
RadioReciever radio(RADIO_CS, RADIO_CE, RF24_PA_MIN, RF24_250KBPS, RADIO_PIPE, RADIO_ADDRESS);

void setup()
{
  Serial.begin(BAUD_RATE);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.setSuffix([](Print* _logOutput){_logOutput->print('\n');});

  configurationManager.setup();
  networkManager.setup();
  mqttClient.setup();
  radio.setup();

  radio.setPacketHandler([&](const RadioReciever::Packet& packet) {
    Log.trace(F("Packet recieved"));
    char topicName[16] = {0};
    sprintf(topicName, "sensors/%d", packet.address);
    mqttClient.publish(topicName, packet.payload, true);
  });
  
}

void loop()
{
  configurationManager.loop();
  networkManager.loop();
  mqttClient.loop();
  radio.loop();
  
  if(mqttClient.cannotConnect())
  {
    configurationManager.requestConfiguration();
  }
}