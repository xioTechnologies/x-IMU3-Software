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
    std::vector<std::pair<int, std::string> > numberEnum{};
    bool readOnly{false};

    std::string value{};
    std::string error{};
    Status status = Status::unknown;

    void clear() {
        sendChangeMessage();

        value = "";
        error = "";
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
            return;
        }

        value = response->value;

        if (response->error.has_value()) {
            error = response->error.value();
            status = Status::errorResponse;
            return;
        }
        //
        // if (checkType(value) != type) {
        //     // TODO: Check type
        //     status = Status::invalidResponse;
        //     return;
        // }

        if (false) {
            // TODO: Check map
            status = Status::invalidResponse;
            return;
        }

        status = Status::confirmed;
    }
};
