#pragma once

#include "../../C/Ximu3.h"

using namespace System;

namespace Ximu3
{
    public ref class Statistics
    {
    internal:
        Statistics(ximu3::XIMU3_Statistics statistics) : statistics{ new ximu3::XIMU3_Statistics{statistics} }
        {
        }

    public:
        ~Statistics()
        {
            delete statistics;
        }

        property UInt64 Timestamp
        {
            UInt64 get()
            {
                return statistics->timestamp;
            }
        }

        property uint64_t DataTotal
        {
            uint64_t get()
            {
                return statistics->data_total;
            }
        }

        property uint32_t DataRate
        {
            uint32_t get()
            {
                return statistics->data_rate;
            }
        }

        property uint64_t MessageTotal
        {
            uint64_t get()
            {
                return statistics->message_total;
            }
        }

        property uint32_t MessageRate
        {
            uint32_t get()
            {
                return statistics->message_rate;
            }
        }

        property uint64_t ErrorTotal
        {
            uint64_t get()
            {
                return statistics->error_total;
            }
        }

        property uint32_t ErrorRate
        {
            uint32_t get()
            {
                return statistics->error_rate;
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_statistics_to_string(*statistics));
        }

    private:
        ximu3::XIMU3_Statistics* statistics;
    };
}
