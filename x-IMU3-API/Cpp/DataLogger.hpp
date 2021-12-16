#pragma once

#include "../C/Ximu3.h"
#include "Helpers.hpp"
#include <string>

namespace ximu3
{
    class DataLogger
    {
    public:
        DataLogger(const std::string& directory, const std::string& name, const std::vector<ximu3::Connection*>& connections, std::function<void(XIMU3_Result)> callback)
        {
            internalCallback = std::move(callback);
            const auto connectionsC = toConnectionsC(connections);
            dataLogger = XIMU3_data_logger_new(directory.c_str(), name.c_str(), connectionsC.data(), (uint32_t) connectionsC.size(), Helpers::wrapCallable<XIMU3_Result>(internalCallback), &internalCallback);
        }

        ~DataLogger()
        {
            XIMU3_data_logger_free(dataLogger);
        }

        static XIMU3_Result log(const std::string& directory, const std::string& name, const std::vector<ximu3::Connection*>& connections, const uint32_t seconds)
        {
            const auto connectionsC = toConnectionsC(connections);
            return XIMU3_data_logger_log(directory.c_str(), name.c_str(), connectionsC.data(), (uint32_t) connectionsC.size(), seconds);
        }

    private:
        XIMU3_DataLogger* dataLogger;
        std::function<void(XIMU3_Result)> internalCallback;

        static std::vector<XIMU3_Connection*> toConnectionsC(const std::vector<ximu3::Connection*>& connections)
        {
            std::vector<XIMU3_Connection*> connectionsC;
            for (auto connection : connections)
            {
                connectionsC.push_back(connection->connection);
            }
            return connectionsC;
        }
    };
} // namespace ximu3
