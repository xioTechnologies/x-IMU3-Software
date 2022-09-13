#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

void GetPortNames()
{
    const XIMU3_CharArrays portNames = XIMU3_port_scanner_get_port_names();
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
