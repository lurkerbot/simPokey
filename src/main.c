#include "libProSimDataSource.h"
#include "main.h"

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

void intHandler(int sig)
{
    char c = '\0';

    signal(sig, SIG_IGN);
    printf("Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
        zlog_fini();

        for (int i = 0; i < numberOfDevices; i++)
        {
            uv_stop(devices[i]->loop);
            PK_DisconnectDevice(devices[i]->pokey);
            free(devices[i]);
        }
        free(simConfig);
        exit(0);
    }
    else
        signal(SIGINT, intHandler);
    getchar(); // Get new line character
}


void* poo(void* data) {
    printf("<------ HERE I AM ----->\n");
    return NULL;
}

int main()
{
    pthread_t cliThread = NULL;
    
    signal(SIGINT, intHandler);
    if (zlog_init(logConfigFile))
    {
        printf("zlog init failed\n");
        return -1;
    }

    logHandler = zlog_get_category("simPokey");
    if (!logHandler)
    {
        printf("get simPokey fail\n");
        zlog_fini();
        return -2;
    }

    zlog_info(logHandler, "---------- STARTING ----------");

    sPoKeysNetworkDeviceSummary *networkDeviceSummary;
    config_setting_t configuredDevices;

    initConfiguration(&configuration, configFile);
    loadConfiguredDevices();
    loadSimulatorDetails();

    zlog_info(logHandler, "Starting network device enumeration...");
    numberOfDevices = PK_EnumerateNetworkDevices(networkDeviceSummary, 800);

    zlog_info(logHandler, "Found %d device(s)", numberOfDevices);

    init_sim_connection(simConfig->ipAddress, simConfig->port, &poo) ;

    int x = 0;
    if (pthread_create(&cliThread, NULL, cliInit, &x))
    {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
   
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

                checkValidPinConfiguration(device, i);
                devices[i] = device;
                applyConfiguration(device);
                dumpDevices();

                // start a thread per device
                int y;
                if (pthread_create(&device->pThread, NULL, startDeviceLoop, &y)){
                    printf("could not start startDeviceLoop thread");
                };
            }
        }
    }
    
    start_sim_loop();

    return 0;
}
