#pragma once

#include "../../C/Ximu3.h"

namespace Ximu3
{
    public enum class ConnectionType
    {
        Usb = ximu3::XIMU3_ConnectionTypeUsb,
        Serial = ximu3::XIMU3_ConnectionTypeSerial,
        Tcp = ximu3::XIMU3_ConnectionTypeTcp,
        Udp = ximu3::XIMU3_ConnectionTypeUdp,
        Bluetooth = ximu3::XIMU3_ConnectionTypeBluetooth,
        File = ximu3::XIMU3_ConnectionTypeFile,
    };
}
