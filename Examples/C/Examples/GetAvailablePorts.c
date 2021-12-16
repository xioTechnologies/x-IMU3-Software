#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

void GetAvailablePorts()
{
    const XIMU3_CharArrays portNames = XIMU3_serial_discovery_get_available_ports();
    if (portNames.length == 0)
    {
        printf("No ports available\n");
        return;
    }
    for (uint32_t index = 0; index < portNames.length; index++)
    {
        printf("%s\n", portNames.array[index]);
    }
    XIMU3_char_arrays_free(portNames);
}
