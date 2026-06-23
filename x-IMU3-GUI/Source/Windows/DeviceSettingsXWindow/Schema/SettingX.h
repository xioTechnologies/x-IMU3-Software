#pragma once

#include <juce_core/juce_core.h>
#include "Ximu3.hpp"
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct SettingX {
    enum class Type {
        string,
        number,
        boolean,
    };

    enum class Status {
        unknown,
        noResponse,
        errorResponse,
        invalidResponse,
        confirmed,
    };

    std::string name;
    std::string key;
    Type type;
    std::vector<std::pair<int, std::string>> numberEnum {};
    bool readOnly{false};

    std::string value {};
    std::string error {};
    Status status = Status::unknown;

    std::string uiValue {};

    static std::optional<Type> checkType(const std::string& value) {
        if (value == "true" || value == "false") {
            return Type::boolean;
        }

        if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
            return Type::string;
        }

        if (value.length() > 0) { // TODO
            return Type::number;
        }

        return {}; // no type
    }

    void receiveResponse(const std::optional<ximu3::CommandMessage> &response) {
        if (response.has_value() == false) {
            status = Status::noResponse;
            return;
        }

        if (response->key != key) {
            return;
        }

        value = response->value;
        uiValue = response->value;

        if (response->error.has_value()) {
            error = response->error.value();
            status = Status::errorResponse;
            return;
        }

        if (checkType(value) != type) {
            // TODO: Check type
            status = Status::invalidResponse;
            return;
        }

        if (false) {
            // TODO: Check map
            status = Status::invalidResponse;
            return;
        }

        status = Status::confirmed;
    }

    std::string readCommand() const {
        return "{\"" + key + "\":null}";
    }

    std::string writeCommand() const {
        if (readOnly) {
            return readCommand();
        }

        const std::string command = "{\"" + key + "\":" + uiValue + "}";

        if (ximu3::CommandMessage::parse(command).has_value() == false) {
            return readCommand();
        }

        return command;
    }
};
