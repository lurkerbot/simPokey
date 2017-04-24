#ifndef __CONFIG_H
#define __CONFIG_H

#include <libconfig.h>

#ifdef __cplusplus
extern “C” {
#endif

typedef struct {
    const char* type;
    const char* ipAddress;
    int port;
} simulator_t;

config_t configuration;
simulator_t* simConfig;

int initConfiguration(config_t *config, const char *fileName);
void* getConfigurationValue(char* element);
int loadConfiguredDevices();
int loadSimulatorDetails();

#ifdef __cplusplus
}
#endif

#endif
