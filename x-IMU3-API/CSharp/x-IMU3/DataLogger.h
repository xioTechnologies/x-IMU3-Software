#pragma once

#include "../../C/Ximu3.h"
#include "Connection.h"
#include "EventArgs.h"
#include "Helpers.h"
#include "Result.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Ximu3
{
    public ref class DataLogger
    {
    public:
        DataLogger(String^ directory, String^ name, array<Connection^>^ connections)
        {
            const auto connectionsC = toConnectionsC(connections);
            dataLogger = ximu3::XIMU3_data_logger_new(Helpers::ToCharPtr(directory), Helpers::ToCharPtr(name), connectionsC.data(), (uint32_t)connectionsC.size());
        }

        ~DataLogger() {
            ximu3::XIMU3_data_logger_free(dataLogger);
        }

        Result GetResult()
        {
            return (Result)ximu3::XIMU3_data_logger_get_result(dataLogger);
        }

        static Result Log(String^ directory, String^ name, array<Connection^>^ connections, int seconds)
        {
            const auto connectionsC = toConnectionsC(connections);
            return (Result)ximu3::XIMU3_data_logger_log(Helpers::ToCharPtr(directory), Helpers::ToCharPtr(name), connectionsC.data(), (uint32_t)connectionsC.size(), seconds);
        }

    private:
        ximu3::XIMU3_DataLogger* dataLogger;

        GCHandle thisHandle = GCHandle::Alloc(this, GCHandleType::Weak);

        static std::vector<ximu3::XIMU3_Connection*> toConnectionsC(array<Connection^>^ connections) {
            std::vector<ximu3::XIMU3_Connection*> connectionsC(connections->Length);
            for (size_t index = 0; index < connectionsC.size(); index++)
            {
                connectionsC[index] = connections[(int)index]->connection;
            }
            return connectionsC;
        }
    };
}
