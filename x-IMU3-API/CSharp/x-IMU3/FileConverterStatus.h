#pragma once

#include "../../C/Ximu3.h"

namespace Ximu3
{
    public enum class FileConverterStatus
    {
        Complete = ximu3::XIMU3_FileConverterStatusComplete,
        Failed = ximu3::XIMU3_FileConverterStatusFailed,
        InProgress = ximu3::XIMU3_FileConverterStatusInProgress,
    };
}
