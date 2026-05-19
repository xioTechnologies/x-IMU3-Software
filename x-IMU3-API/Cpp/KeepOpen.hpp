#pragma once

#include "../C/Ximu3.h"
#include "Connection.hpp"
#include "Helpers.hpp"

namespace ximu3 {
    class KeepOpen {
    public:
        KeepOpen(Connection &connection) {
            wrapped = XIMU3_keep_open_new(connection.wrapped);
        }

        ~KeepOpen() {
            XIMU3_keep_open_free(wrapped);
        }

    private:
        XIMU3_KeepOpen *wrapped;
    };
} // namespace ximu3
