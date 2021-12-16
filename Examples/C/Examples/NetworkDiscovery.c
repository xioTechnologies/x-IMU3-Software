#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

static void Callback(const XIMU3_DiscoveredNetworkDevices devices, void* context);

static void PrintDevices(const XIMU3_DiscoveredNetworkDevices devices);

void NetworkDiscovery()
{
    if (YesOrNo("Use async implementation?") == true)
    {
        XIMU3_NetworkDiscovery* const discovery = XIMU3_network_discovery_new(Callback, NULL);
        Wait(-1);
        XIMU3_network_discovery_free(discovery);
    }
    else
    {
        const XIMU3_DiscoveredNetworkDevices devices = XIMU3_network_discovery_scan(2000);
        printf("Discovered %u devices\n", devices.length);
        PrintDevices(devices);
        XIMU3_discovered_network_devices_free(devices);
    }
}

static void Callback(const XIMU3_DiscoveredNetworkDevices devices, void* context)
{
    printf("Devices updated (%u devices available)\n", devices.length);
    PrintDevices(devices);
    XIMU3_discovered_network_devices_free(devices);
}

static void PrintDevices(const XIMU3_DiscoveredNetworkDevices devices)
{
    for (uint32_t index = 0; index < devices.length; index++)
    {
        const XIMU3_DiscoveredNetworkDevice* const device = &devices.array[index];
        printf("%s - %s, RSSI: %u%%, Battery: %u%%, %s, %s, %s\n",
               device->device_name,
               device->serial_number,
               device->rssi,
               device->battery,
               XIMU3_charging_status_to_string(device->status),
               XIMU3_tcp_connection_info_to_string(device->tcp_connection_info),
               XIMU3_udp_connection_info_to_string(device->udp_connection_info));
        // printf("%s\n", XIMU3_discovered_network_device_to_string(*device)); // alternative to above
    }
}
