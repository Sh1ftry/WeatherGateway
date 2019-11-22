#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include "Configuration.h"
#include "Runnable.h"

class ConfigurationManager: public Runnable
{
    public:
        ConfigurationManager(const char* filename): filename(filename) {};
        void setup() override;
        void loop() override {};
        void requestConfiguration();
        inline const Configuration& getConfiguration() const
        {
            return this->configuration;
        }
    private:
        const char* filename;
        Configuration configuration;
        bool (*shouldRequestConfiguration)();

        void saveConfiguration();
        bool loadConfiguration();
};

#endif /* CONFIUGURATION_MANAGER_H */