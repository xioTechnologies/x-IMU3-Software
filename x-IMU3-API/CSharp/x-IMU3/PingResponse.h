#pragma once

#include "../../C/Ximu3.h"
#include "Result.h"

using namespace System;

namespace Ximu3
{
    public ref class PingResponse
    {
    internal:
        PingResponse(ximu3::XIMU3_PingResponse pingResponse) : pingResponse{ new ximu3::XIMU3_PingResponse{pingResponse} }
        {
        }

    public:
        ~PingResponse()
        {
            delete pingResponse;
        }

        property Result Result
        {
            Ximu3::Result get()
            {
                return (Ximu3::Result)pingResponse->result;
            }
        }

        property String^ Interface
        {
            String^ get()
            {
                return gcnew String(pingResponse->interface);
            }
        }

        property String^ DeviceName
        {
            String^ get()
            {
                return gcnew String(pingResponse->device_name);
            }
        }

        property String^ SerialNumber
        {
            String^ get()
            {
                return gcnew String(pingResponse->serial_number);
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_ping_response_to_string(*pingResponse));
        }

    private:
        ximu3::XIMU3_PingResponse* pingResponse;
    };
}
