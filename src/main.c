#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "PoKeysLib.h"
#include "PoKeysDevices.h"
#include "config.h"

const int MAX_DEVICES = 16;
const char *configFile = "config.cfg";

int dumpDeviceSummary(sPoKeysNetworkDeviceSummary device)
{
    printf("\tSerial Number: %d\n", device.SerialNumber);
    printf("\tIP Address: %d.%d.%d.%d\n", device.IPaddress[0], device.IPaddress[1], device.IPaddress[2], device.IPaddress[3]);
    printf("\tHost IP Address: %d.%d.%d.%d\n", device.hostIP[0], device.hostIP[1], device.hostIP[2], device.hostIP[3]);
    printf("\tFirmware: %d.%d\n", device.FirmwareVersionMajor, device.FirmwareVersionMinor);
    printf("\tUserId: %d\n", device.UserID);
    printf("\tHardware: %d\n", device.HWtype);

    return 1;
}

void connectToDevice(serialNumber, checkForDevice)
{
    sPoKeysDevice *device = PK_ConnectToDeviceWSerial(serialNumber, checkForDevice);

    printf("%s\n", device->DeviceData.DeviceTypeName);
    printf("%s\n", device->DeviceData.BuildDate);
}

int main()
{
    const char *configFile = "config.cfg";
    sPoKeysNetworkDeviceSummary networkDeviceSummary;

    initConfiguration(&configuration, configFile);

    // config_setting_t* configuredDevices;
    // int configDevices = getConfigurationList(configuredDevices, "configuration.devices");

    //  printf("%d\n",(int)configuredDevices->value.list->length);


    int numberOfDevices = PK_EnumerateNetworkDevices(&networkDeviceSummary, 800);

    printf("Detected: %d devices\n", numberOfDevices);

    for (int i = 0; i < numberOfDevices; i++)
    {
        dumpDeviceSummary(networkDeviceSummary);
        connectToDevice(networkDeviceSummary.SerialNumber, 1);
    }

    return 0;
}