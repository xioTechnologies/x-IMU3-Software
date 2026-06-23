#pragma once

#include "../C/Ximu3.h"
#include "Connection.hpp"
#include <string>

namespace ximu3 {
    inline XIMU3_Result settings_backup(const std::string &file_path, const Connection &connection) {
        return XIMU3_settings_backup(file_path.c_str(), connection.wrapped);
    }

    inline XIMU3_Result settings_restore(const std::string &file_path, const Connection &connection) {
        return XIMU3_settings_restore(file_path.c_str(), connection.wrapped);
    }
} // namespace ximu3
