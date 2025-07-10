#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <regex>
#include "Ximu3.hpp"

class CommandMessage
{
public:
    std::string json;
    std::string key;
    std::string value;
    std::optional<std::string> error;

    CommandMessage(const juce::String& key_, const juce::var& value_)
        : json("{\"" + key_.toStdString() + "\":" + juce::JSON::toString(value_).toStdString() + "}"),
          key(key_.toStdString()),
          value(juce::JSON::toString(value_).toStdString())
    {
    }

    explicit CommandMessage(const std::string& json_) : json(json_)
    {
        const auto commandMessage = ximu3::XIMU3_command_message_parse(json.c_str());
        json = commandMessage.json;
        key = commandMessage.key;
        value = commandMessage.value;
        if (std::strlen(commandMessage.error) > 0)
        {
            error = commandMessage.error;
        }
    }

    bool operator==(const CommandMessage& other) const
    {
        return key.empty() == false && key == other.key; // empty key in response indicates no response
    }

    operator const std::string&() const
    {
        return json;
    }

    juce::var getValue() const
    {
        return juce::JSON::fromString(value);
    }

    static juce::String normaliseKey(const juce::String& key)
    {
        return std::regex_replace(key.toLowerCase().toStdString(), std::regex("[^0-9a-z]"), "");
    }

private:
    JUCE_LEAK_DETECTOR(CommandMessage)
};
