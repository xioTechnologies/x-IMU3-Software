#include "Connection.hpp"
#include "ConnectionConfig.hpp"

namespace ximu3 {
    MuxConnectionConfig::MuxConnectionConfig(const std::uint8_t channel, const Connection &connection) {
        muxConnectionConfig = XIMU3_mux_connection_config_new(channel, connection.wrapped);
    }
} // namespace ximu3
