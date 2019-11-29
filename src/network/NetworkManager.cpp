#include <WiFiManager.h>
#include <ArduinoLog.h>
#include "../../include/network/NetworkManager.h"

void NetworkManager::setup()
{
    WiFi.setAutoReconnect(true);
    this->connect();
    lastConnectionCheckMs = millis();
}

void NetworkManager::loop()
{
    if(lastConnectionCheckMs + 10000 < millis())
    {
        this->connect();
        lastConnectionCheckMs = millis();
    }
}

void NetworkManager::connect()
{
    if(WiFi.status() != WL_CONNECTED)
    {
        Log.warning(F("Not connected to WiFi"));
        WiFiManager wifiManager;
        wifiManager.autoConnect();
    }
}