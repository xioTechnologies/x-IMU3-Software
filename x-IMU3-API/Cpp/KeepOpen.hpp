#pragma once

#include "../C/Ximu3.h"
#include "Connection.hpp"
#include "Helpers.hpp"

namespace ximu3 {
    class KeepOpen {
    public:
        KeepOpen(Connection &connection, std::function<void(XIMU3_ConnectionStatus)> callback_) {
            callback = std::move(callback_);
            wrapped = XIMU3_keep_open_new(connection.wrapped, Helpers::wrapCallable<XIMU3_ConnectionStatus>(callback), &callback);
        }

        ~KeepOpen() {
            XIMU3_keep_open_free(wrapped);
        }

    private:
        XIMU3_KeepOpen *wrapped;
        std::function<void(XIMU3_ConnectionStatus)> callback;
    };
} // namespace ximu3
