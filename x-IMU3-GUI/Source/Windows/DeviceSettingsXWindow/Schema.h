#pragma once

#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "Ximu3.hpp"

class Schema {
public:
    struct Setting : juce::ChangeBroadcaster {
        enum class Type {
            string,
            number,
            enumeration,
            boolean,
        };

        enum class Status {
            unknown,
            noResponse,
            errorResponse,
            invalidResponse,
            confirmed,
        };

        std::string key;
        Type type;
        std::vector<std::pair<int, std::string> > enumeration;
        bool readOnly;

        std::string value;
        std::string error;
        Status status = Status::unknown;

        std::string name;
        std::string emptyString;
        std::string dependsOnKey;
        std::vector<std::string> dependsOnValues;

        Setting(const juce::ValueTree &tree, const juce::ValueTree &enums);

        void clear();

        std::string getRead() const;

        std::string getWrite(const std::string &value_) const;

        void receive(const std::optional<ximu3::CommandMessage> &response);

        static ximu3::XIMU3_JsonType jsonTypeFrom(const Type type_);

        static Type typeFrom(const juce::String &type);
    };

    struct Group {
        std::string name;
        bool expand;
        std::string dependsOnKey;
        std::vector<std::string> dependsOnValues;

        std::vector<std::variant<Group, std::unique_ptr<Setting> >> items;

        Group(const juce::ValueTree &tree, const juce::ValueTree &enums);

        std::vector<Setting *> flatten() const;

        Setting *find(const std::string &key) const;
    };

    static Group load(const juce::ValueTree &tree);

private:
    static std::vector<std::string> vectorFrom(const juce::String& string);
};
