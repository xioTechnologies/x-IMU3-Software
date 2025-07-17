#include "ConnectionInfo.hpp"
#include "Connection.hpp"

namespace ximu3
{
    MuxConnectionInfo::MuxConnectionInfo(const std::uint8_t channel, const Connection& connection)
    {
        muxConnectionInfo = XIMU3_mux_connection_info_new(channel, connection.connection);
    }
} // namespace ximu3
