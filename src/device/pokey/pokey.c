#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include <uv.h>
#include "../../encoder/encoder.h"
#include "../../pin/pin.h"
#include "pokey.h"

//forward decl
void *logHandler;

int syncDeviceName(device_t *device)
{
    if (strncmp((char *)device->pokey->DeviceData.DeviceName, device->name, MAX_DEVICE_NAME_LENGTH) != 0)
    {
        memcpy(device->pokey->DeviceData.DeviceName, device->name, MAX_DEVICE_NAME_LENGTH);
        int ret = PK_DeviceNameSet(device->pokey);

        if (ret != PK_OK)
        {
            printf("PK_DeviceNameSet: Err %d\n", ret);
            return -1;
        }

        zlog_info(logHandler, " - Reset device name %s", device->name);
    }

    return 1;
}

void dumpDevice(device_t *device)
{
    printf("\nDevice Name: %s (#%s)\n", device->name, device->serialNumber);

    if (device->hasPokey)
    {

        printf("Device Type: %s(%d) ", device->pokey->DeviceData.DeviceTypeName, device->pokey->DeviceData.UserID);
        printf("Firmware: v%d.%d.%d ", (device->pokey->DeviceData.FirmwareVersionMajor >> 4) + 1,
               device->pokey->DeviceData.FirmwareVersionMajor & 0x0F,
               device->pokey->DeviceData.FirmwareVersionMinor);

        printf("IP Address: %d.%d.%d.%d\n", (int)device->pokey->netDeviceData->IPAddressCurrent[0],
               (int)device->pokey->netDeviceData->IPAddressCurrent[1],
               (int)device->pokey->netDeviceData->IPAddressCurrent[2],
               (int)device->pokey->netDeviceData->IPAddressCurrent[3]);

        printf("Pins: %d ", device->pokey->info.iPinCount);
        printf("Analog: %d ", device->pokey->info.iAnalogInputs);
        printf("PWM: %d/%dMhz ", device->pokey->info.iPWMCount, (device->pokey->info.PWMinternalFrequency) / 1000000);
        printf("Encoders: %d (%db/%df/%duf)\n\n", device->pokey->info.iEncodersCount,
               device->pokey->info.iBasicEncoderCount,
               device->pokey->info.iFastEncoders,
               device->pokey->info.iUltraFastEncoders);

        // dumpEncoders(device->pokey);
        // PK_PinConfigurationGet(device->pokey);
        // printf("Current configuration on Pokey\n\n");
        // printf("%9s  %9s %9s %9s \n", "pin", "Use", "type", "default");
        // printf("%9s %9s %9s %9s \n", "-----", "---", "----", "-------");

        // for (int i = 0; i < device->pokey->info.iPinCount - 1; i++)
        // {
        //     if (device->pokey->Pins[i].PinFunction == PK_PinCap_pinRestricted)
        //         continue;

        //     printf("%9d %9s\n", i + 1, (char *)getPinFunction(device->pokey->Pins[i].PinFunction));
        // }
    }

    printf("%9s %16.16s %9s %9s %9s \n", "index", "name", "pin", "type", "default");
    printf("%9s %16.16s %9s %9s %9s \n", "-----", "----", "---", "----", "-------");

    for (int y = 0; y < device->numberOfPins; y++)
    {
        device_port_t *port = device->pins[y];
        printf("%9i %16.16s %9d %9s %9d \n", y, port->name,
               port->pin,
               (char *)getPinTypeString(port->type),
               port->defaultValue);
    }
}

void dumpDevices()
{
    zlog_info(logHandler, "Dumping device to stdout");

    for (int i = 0; i < activeDevices; i++)
    {
        dumpDevice(devices[i]);
    }
}

int getDeviceBySerialNumber(device_t *device, char *serialNumber)
{
    for (int i = 0; i < numberOfDevices ; i++)
    {

        device_t *searchDevice = devices[i];
            
        if (strcmp(serialNumber, searchDevice->serialNumber) == 0)
        {
            memcpy(device, searchDevice, sizeof(device_t));
            return 1;
        }
    }
    return 0;
}

int applyConfiguration(device_t *device)
{

    applyPinConfigurationToDevice(device);
    return 0;
}

void digitalIOTimerCallback(uv_timer_t *timer, int status)
{
    device_t *device = timer->data;
    sPoKeysDevice *pokey = device->pokey;

    /** read in all the digital IO from the pokey **/
    int ret = PK_DigitalIOGet(pokey);

    if (ret == PK_OK)
    {

        /** iterate over the pins **/
        for (int i = 0; i < device->numberOfPins; i++)
        {
            int devicePin = device->pins[i]->pin-1;
            if(device->pins[i]->type != DIGITAL_INPUT){
                continue;
            }

            if (device->pins[i]->value != pokey->Pins[devicePin].DigitalValueGet)
            {
                device->pins[i]->previousValue = device->pins[i]->value;
                device->pins[i]->value = pokey->Pins[devicePin].DigitalValueGet;
                zlog_info(logHandler, "%s - %s - %s --> %s", device->name,
                          device->pins[i]->name,
                          device->pins[i]->previousValue == 1 ? "On" : "Off",
                          device->pins[i]->value == 1 ? "On" : "Off");
            }
        }
    }
}


int startDeviceLoop(device_t *device)
{

    uv_timer_t digitalIOTimer;
    uint64_t freq = DEVICE_READ_INTERVAL;

    device->loop = uv_loop_new();
    digitalIOTimer.data = device;
    uv_timer_init(device->loop, &digitalIOTimer);

    int ret = uv_timer_start(&digitalIOTimer, (uv_timer_cb)&digitalIOTimerCallback, 100, freq);

    zlog_info(logHandler,"Starting processing....");

    if (ret == 0)
        uv_run(device->loop, UV_RUN_DEFAULT);

    return 1;
}
