#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

void get_port_names()
{
    const XIMU3_CharArrays port_names = XIMU3_port_scanner_get_port_names();

    if (port_names.length == 0)
    {
        printf("No ports available\n");
        return;
    }

    for (uint32_t index = 0; index < port_names.length; index++)
    {
        printf("%s\n", port_names.array[index]);
    }

    XIMU3_char_arrays_free(port_names);
}
