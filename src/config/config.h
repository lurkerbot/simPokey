#ifndef __CONFIG_H
#define __CONFIG_H

#include <libconfig.h>


config_t configuration;

int initConfiguration(config_t *config, const char *fileName);
void* getConfigurationValue(char* element);
int loadConfiguredDevices();

#endif
