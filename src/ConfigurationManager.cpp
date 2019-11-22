#include <WiFiManager.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#include "Configuration.h"
#include "ConfigurationManager.h"

void ConfigurationManager::setup()
{
    if(!this->loadConfiguration())
    {
        this->requestConfiguration();
    }
}

void ConfigurationManager::requestConfiguration()
{
    WiFiManager wifiManager;
    WiFiManagerParameter hostParam("mqtt_host", "Hostname", this->configuration.getMqttHost(), CONF_HOST_LENGTH);
    WiFiManagerParameter usernameParam("mqtt_username", "Username", this->configuration.getMqttUsername(), CONF_USERNAME_LENGTH);
    WiFiManagerParameter passwordParam("mqtt_password", "Password", this->configuration.getMqttPassword(), CONF_PASSWORD_LENGTH);
    
    char portString[7] = {0};
    sprintf(portString, "%d", this->configuration.getMqttPort());
    WiFiManagerParameter portParam("mqtt_port", "Port", portString, 6);

    wifiManager.addParameter(&hostParam);
    wifiManager.addParameter(&portParam);
    wifiManager.addParameter(&usernameParam);
    wifiManager.addParameter(&passwordParam);

    Log.trace(F("Starting configuration portal"));
    wifiManager.startConfigPortal();

    this->configuration.setMqttHost(hostParam.getValue());
    this->configuration.setMqttPassword(passwordParam.getValue());
    this->configuration.setMqttUsername(usernameParam.getValue());
    int port;
    sscanf(portParam.getValue(), "%d", &port);
    this->configuration.setMqttPort(port);
    
    this->saveConfiguration();
}

void ConfigurationManager::saveConfiguration()
{
    if(SPIFFS.begin())
    {
        Log.trace(F("Saving configuration to SPIFFS"));
        SPIFFS.remove(this->filename);
        
        File configuration = SPIFFS.open(this->filename, "w");
        
        StaticJsonDocument<256> document;
        document["port"] = this->configuration.getMqttPort();
        document["host"] = this->configuration.getMqttHost();
        document["username"] = this->configuration.getMqttUsername();
        document["password"] = this->configuration.getMqttPassword();
        
        if(!serializeJson(document, configuration))
        {
            Log.error(F("Could not write configuration to file"));
        }
        
        configuration.close();
    }
    else
    {
        Log.error(F("Could not initialize SPIFFS"));
    }
}

bool ConfigurationManager::loadConfiguration()
{
    bool available = false;
    if(SPIFFS.begin())
    {
        Log.notice(F("Loading configuration from SPIFFS"));
        if(SPIFFS.exists(this->filename))
        {
            File file = SPIFFS.open(this->filename, "r");
            StaticJsonDocument<256> document;
            DeserializationError error = deserializeJson(document, file);
            if(!error)
            {
                this->configuration.setMqttPort(document["port"]);
                this->configuration.setMqttHost(document["host"]);
                this->configuration.setMqttUsername(document["username"]);
                this->configuration.setMqttPassword(document["password"]);
                if(strlen(this->configuration.getMqttHost()) > 0
                    && this->configuration.getMqttPort() != 0)
                {
                    available = true;
                }
            }
            else
            {
                Log.error(F("Could not deserialize configuration file"));
                Log.error(error.c_str());
            } 
            file.close();
        }
        else
        {
            Log.warning(F("Configuration file does not exist"));
        }
    }
    else
    {
        Log.error(F("Could not initialize SPIFFS"));
    }
    
    return available;
}