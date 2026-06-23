#pragma once

#include "../C/Ximu3.h"
#include <cstring>
#include <optional>
#include <string>

namespace ximu3 {
    struct CommandMessage {
        std::string json;
        std::string key;
        std::string value;
        std::optional<std::string> error;

        static std::optional<CommandMessage> from(const XIMU3_CommandMessage &response_) {
            if (std::strlen(response_.json) == 0) {
                return {};
            }

            CommandMessage response;
            response.json = response_.json;
            response.key = response_.key;
            response.value = response_.value;

            if (std::strlen(response_.error) != 0) {
                response.error = response_.error;
            }

            return response;
        }

        static std::optional<CommandMessage> parse(const std::string &json) {
            return from(XIMU3_command_message_parse(json.c_str()));
        }
    };
}
