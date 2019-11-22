#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define CONF_HOST_LENGTH 64
#define CONF_USERNAME_LENGTH 64
#define CONF_PASSWORD_LENGTH 64

class Configuration
{
    public:
    inline int getMqttPort() const
    {
        return mqttPort;
    }
    inline const char* getMqttHost() const
    {
        return mqttHost;
    }
    inline const char* getMqttUsername() const
    {
        return mqttUsername;
    }
    inline const char* getMqttPassword() const
    {
        return mqttPassword;
    }

    inline void setMqttPort(int port)
    {
        this->mqttPort = port;
    }

    inline void setMqttHost(const char* host)
    {
        snprintf(mqttHost, CONF_HOST_LENGTH, "%s", host);
    }

    inline void setMqttUsername(const char* username)
    {
        snprintf(mqttUsername, CONF_USERNAME_LENGTH, "%s", username);
    }

    inline void setMqttPassword(const char* password)
    {
        snprintf(mqttPassword, CONF_PASSWORD_LENGTH, "%s", password);
    }

    private:
    int mqttPort;
    char mqttHost[CONF_HOST_LENGTH];
    char mqttUsername[CONF_USERNAME_LENGTH];
    char mqttPassword[CONF_PASSWORD_LENGTH];
};

#endif /* CONFIGURATION_H */