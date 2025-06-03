#pragma once

#include "../C/Ximu3.h"
#include "Helpers.hpp"
#include "Connection.hpp"

namespace ximu3
{
    class KeepOpen
    {
    public:
        KeepOpen(Connection& connection, std::function<void(XIMU3_ConnectionStatus)> callback)
        {
            internalCallback = std::move(callback);
            keepOpen = XIMU3_keep_open_new(connection.connection, Helpers::wrapCallable<XIMU3_ConnectionStatus>(internalCallback), &internalCallback);
        }

        ~KeepOpen()
        {
            XIMU3_keep_open_free(keepOpen);
        }

    private:
        XIMU3_KeepOpen* keepOpen;
        std::function<void(XIMU3_ConnectionStatus)> internalCallback;
    };
} // namespace ximu3
