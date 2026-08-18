#pragma once

#include <juce_core/juce_core.h>
#include "Ximu3.hpp"
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct SettingX : juce::ChangeBroadcaster {
    enum class Type {
        string,
        number,
        enumeration,
        boolean,
    };

    static ximu3::XIMU3_JsonType jsonTypeFrom(const Type type_) {
        switch (type_) {
            case Type::string:
                return ximu3::XIMU3_JsonTypeString;
            case Type::number:
            case Type::enumeration:
                return ximu3::XIMU3_JsonTypeNumber;
            case Type::boolean:
                return ximu3::XIMU3_JsonTypeBoolean;
        }

        return {}; // avoid compiler warning
    }

    enum class Status {
        unknown,
        noResponse,
        errorResponse,
        invalidResponse,
        confirmed,
    };

    const std::string key{};
    const Type type{};
    const std::vector<std::pair<int, std::string> > enumeration{};
    const bool readOnly{};

    std::string value{};
    std::string error{};
    Status status = Status::unknown;

    const std::string name{};
    const std::string emptyString{};
    const std::string dependsOnKey{};
    const std::vector<std::string> dependsOnValues{};

    void clear() {
        sendChangeMessage();

        value = {};
        error = {};
        status = Status::unknown;
    }

    std::string getRead() const {
        return "{\"" + key + "\":null}";
    }

    std::string getWrite(const std::string &value_) const {
        if (readOnly) {
            return getRead();
        }

        const std::string command = "{\"" + key + "\":" + value_ + "}";

        if (ximu3::CommandMessage::parse(command).has_value() == false) {
            return getRead();
        }

        return command;
    }

    void receive(const std::optional<ximu3::CommandMessage> &response) {
        sendChangeMessage();

        if (response.has_value() == false) {
            status = Status::noResponse;
            return;
        }

        if (response->key != key) {
            return; // TODO: Delete this?
        }

        if (response->error.has_value()) {
            error = response->error.value();
            status = Status::errorResponse;
            return;
        }

        if (jsonTypeFrom(type) != response->valueType) {
            status = Status::invalidResponse;
            return;
        }

        if (false) {
            // TODO: Check map
            status = Status::invalidResponse;
            return;
        }

        value = response->value;
        status = Status::confirmed;
    }
};
