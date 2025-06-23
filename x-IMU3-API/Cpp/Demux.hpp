#pragma once

#include "../C/Ximu3.h"
#include "Connection.hpp"
#include <cstdint>

namespace ximu3
{
    class Demux
    {
    public:
        Demux(Connection& connection, const std::vector<std::uint8_t>& channels)
        {
            demux = XIMU3_demux_new(connection.connection, channels.data(), channels.size());
        }

        ~Demux()
        {
            XIMU3_demux_free(demux);
        }

        XIMU3_Result getResult()
        {
            return XIMU3_demux_get_result(demux);
        }

    private:
        XIMU3_Demux* demux;
    };
} // namespace ximu3
