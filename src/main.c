#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <zlog.h>
#include "PoKeysLib.h"
#include "config.h"
#include "devices.h"


const char *configFile = "../config/config.cfg";
const char *logConfigFile = "../config/zlog.conf";
zlog_category_t *logHandler;


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
   
    if (zlog_init(logConfigFile)) {
		printf("init failed\n");
		return -1;
	}

    logHandler = zlog_get_category("simPokey");
    if (!logHandler) {
		printf("get cat fail\n");
		zlog_fini();
		return -2;
	}

    zlog_info(logHandler, "---------- STARTING ----------");



    sPoKeysNetworkDeviceSummary networkDeviceSummary;
    config_setting_t configuredDevices;
    
    
    initConfiguration(&configuration, configFile);

    //*****
    
    loadConfiguredDevices();


    zlog_info(logHandler, "Starting network device enumeration...");
    int numberOfDevices = PK_EnumerateNetworkDevices(&networkDeviceSummary, 800);

    zlog_info(logHandler,"Detected: %d devices\n", numberOfDevices);

    // for (int i = 0; i < numberOfDevices; i++)
    // {
    //     dumpDeviceSummary(networkDeviceSummary);
    //     connectToDevice(networkDeviceSummary.SerialNumber, 1);
    // }
    zlog_fini();

    return 0;
}