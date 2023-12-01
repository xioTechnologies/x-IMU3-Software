// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#pragma once

#include "../../../C/Ximu3.h"

using namespace System;

namespace Ximu3
{
    public ref class AhrsStatusMessage
    {
    internal:
        AhrsStatusMessage(ximu3::XIMU3_AhrsStatusMessage message) : message{ new ximu3::XIMU3_AhrsStatusMessage{message} }
        {
        }

    public:
        ~AhrsStatusMessage()
        {
            delete message;
        }

        property UInt64 Timestamp
        {
            UInt64 get()
            {
                return message->timestamp;
            }
        }

        property float Initialising
        {
            float get()
            {
                return message->initialising;
            }
        }

        property float AngularRateRecovery
        {
            float get()
            {
                return message->angular_rate_recovery;
            }
        }

        property float AccelerationRecovery
        {
            float get()
            {
                return message->acceleration_recovery;
            }
        }

        property float MagneticRecovery
        {
            float get()
            {
                return message->magnetic_recovery;
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_ahrs_status_message_to_string(*message));
        }

    private:
        ximu3::XIMU3_AhrsStatusMessage* message;
    };
}