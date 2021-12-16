#pragma once

#include "../../C/Ximu3.h"

namespace Ximu3
{
    public enum class ChargingStatus
    {
        NotConnected = ximu3::XIMU3_ChargingStatusNotConnected,
        Charging = ximu3::XIMU3_ChargingStatusCharging,
        ChargingComplete = ximu3::XIMU3_ChargingStatusChargingComplete,
    };
}
