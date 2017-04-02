#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <zlog.h>
#include <uv.h>
#include "PoKeysLib.h"
#include "device/pokey/pokey.h"
#include "config/config.h"
#include "pin/pin.h"

const char *configFile = "../config/config.cfg";
const char *logConfigFile = "../config/zlog.conf";
zlog_category_t *logHandler;

int connectToDevice(sPoKeysNetworkDeviceSummary *networkDevice, sPoKeysDevice *pokey)
{
    zlog_info(logHandler, " - Connecting to device %d", networkDevice->SerialNumber);
    sPoKeysDevice *device = PK_ConnectToNetworkDevice(networkDevice);
    if (device == NULL)
    {
        zlog_info(logHandler, " - Unable to connect");
        return 0;
    }
    else
    {
        zlog_info(logHandler, " - Found %s (%s - %d)", device->DeviceData.DeviceName, device->DeviceData.DeviceTypeName, device->DeviceData.UserID);
        memcpy(pokey, device, sizeof(sPoKeysDevice));
        return 1;
    }
    return 0;
}

int main()
{

    if (zlog_init(logConfigFile))
    {
        printf("zlog init failed\n");
        return -1;
    }

    logHandler = zlog_get_category("simPokey");
    if (!logHandler)
    {
        printf("get cat fail\n");
        zlog_fini();
        return -2;
    }

    zlog_info(logHandler, "---------- STARTING ----------");

    sPoKeysNetworkDeviceSummary *networkDeviceSummary;
    config_setting_t configuredDevices;

    initConfiguration(&configuration, configFile);

    //*****

    loadConfiguredDevices();

    zlog_info(logHandler, "Starting network device enumeration...");
    int numberOfDevices = PK_EnumerateNetworkDevices(networkDeviceSummary, 800);

    zlog_info(logHandler, "Found %d device(s)", numberOfDevices);

    for (int i = 0; i < numberOfDevices; i++)
    {

        device_t *device = malloc(sizeof(device_t));
        char serialNumberString[128];

        sPoKeysNetworkDeviceSummary networkDevice = networkDeviceSummary[i];

        // convert networkDevice->SerialNumber integer into a string
        snprintf(serialNumberString, 128, "%d", (unsigned int)networkDevice.SerialNumber);

        if (getDeviceBySerialNumber(device, (void *)&serialNumberString))
        {
            device->pokey = malloc(sizeof(sPoKeysDevice));
            int ret = connectToDevice(&networkDevice, device->pokey);
            if (ret)
            {
                device->hasPokey = 1;
                /** check the name on the pokey and update **/
                syncDeviceName(device);
                checkValidPinConfiguration(device, 1);
                applyConfiguration(device);
                startDeviceLoop(device);
                devices[i] = device;
            }
        }
    }

    dumpDevices();
    zlog_fini();

    for (int i = 0; i < numberOfDevices; i++)
    {
        PK_DisconnectDevice(devices[i]->pokey);
        uv_stop(devices[i]->loop);
    }

    return 0;
}
