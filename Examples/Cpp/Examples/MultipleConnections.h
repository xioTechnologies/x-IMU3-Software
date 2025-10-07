#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class MultipleConnections
{
public:
    MultipleConnections()
    {
        ximu3::NetworkAnnouncement networkAnnouncement;

        const auto result = networkAnnouncement.getResult();

        if (result != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Network announcement failed. " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        std::vector<std::unique_ptr<ximu3::Connection>> connections;
        for (const auto& message : networkAnnouncement.getMessagesAfterShortDelay())
        {
            const ximu3::UdpConnectionInfo connectionInfo (message);

            auto connection = std::make_unique<ximu3::Connection>(connectionInfo);

            if (connection->open() != ximu3::XIMU3_ResultOk)
            {
                std::cout << "Unable to open " << connectionInfo.toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
                return;
            }

            connection->addStatisticsCallback(statisticsCallback);

            connections.push_back(std::move(connection));
        }

        std::this_thread::sleep_for(std::chrono::hours(48));
    }

private:
    std::function<void(ximu3::XIMU3_Statistics statistics)> statisticsCallback = [](auto statistics)
    {
        printf(TIMESTAMP_FORMAT UINT64_FORMAT " bytes" UINT32_FORMAT " bytes/s" UINT64_FORMAT " messages" UINT32_FORMAT " messages/s" UINT64_FORMAT " errors" UINT32_FORMAT " errors/s\n",
               statistics.timestamp,
               statistics.data_total,
               statistics.data_rate,
               statistics.message_total,
               statistics.message_rate,
               statistics.error_total,
               statistics.error_rate);
        // std::cout << ximu3::XIMU3_statistics_to_string(statistics) << std::endl; // alternative to above
    };
};
