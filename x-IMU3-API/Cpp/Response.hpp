#pragma once

#include "../C/Ximu3.h"
#include <optional>
#include <string>

namespace ximu3
{
    struct Response
    {
        std::string json;
        std::string key;
        std::string value;
        std::optional<std::string> error;

        static std::optional<Response> fromC(XIMU3_Response responseC)
        {
            if (std::strlen(responseC.json) > 0)
            {
                Response response;
                response.json = responseC.json;
                response.key = responseC.key;
                response.value = responseC.value;
                if (std::strlen(responseC.error) > 0)
                {
                    response.error = responseC.error;
                }
                return response;
            }

           return {};
        }
    };
} // namespace ximu3
