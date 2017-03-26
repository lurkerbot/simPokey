#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <string.h>
#include "config.h"

int configIsLoaded = 0;

int initConfiguration(config_t *config, const char *fileName)
{
    config_init(config);
    if (!config_read_file(config, fileName))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(config),
                config_error_line(config), config_error_text(config));
        config_destroy(config);
        return (EXIT_FAILURE);
    }

    configIsLoaded = 1;

    printf("Loaded config: %s (v%s)\n", fileName, getConfigurationValue("version"));

    return (CONFIG_TRUE);
}

void *getConfigurationValue(char *element)
{
    const char *str;
    if (config_lookup_string(&configuration, element, &str))
        return (char *)str;
    else
        fprintf(stderr, "No %s setting in configuration file.\n", element);

    return 0;
}

int getConfigurationList(config_setting_t *items, char *element)
{
    config_setting_t* list = config_lookup(&configuration, element);
    int count = config_setting_length(list);

    printf("from list %d\n",list->value.list->length);

    //items = malloc(sizeof(config_setting_t)*count);
    memcpy(&items, &list, sizeof(sizeof(config_setting_t)*count));
    printf("from items %d\n",items->value.list->length);

    
    return 0;
}
