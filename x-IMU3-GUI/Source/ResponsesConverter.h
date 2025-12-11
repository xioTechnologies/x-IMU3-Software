#pragma once

#include "Ximu3.hpp"
#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>
#include <string>

class ResponsesConverter
{
public:
    class CommandMessage
    {
    public:
        std::string json;
        std::string key;
        std::string value;
        std::optional<std::string> error;
    };

    static std::vector<std::optional<CommandMessage>> convert (const std::vector<std::string>& commands, const std::vector<std::string>& responses)
    {
        std::vector<std::optional<CommandMessage>> converted;

        for (auto& key : getKeys(commands))
        {
            converted.push_back(findResponse(responses, key));
        }

        return converted;
    }

private:
    static std::string getKey(const std::string& command)
    {
        const auto var = juce::JSON::parse(command);

        if (const auto* const object = var.getDynamicObject())
        {
            return object->getProperties().getName(0).toString().toStdString();
        }

        jassertfalse;
        return {};
    }

    static std::vector<std::string> getKeys(const std::vector<std::string>& commands)
    {
        std::vector<std::string> keys;
        for (auto& command : commands)
        {
            keys.push_back(getKey(command));
        }
        return keys;
    }

    static std::optional<CommandMessage> findResponse(const std::vector<std::string>& responses, const std::string& key)
    {
        for (const auto& response : responses)
        {
            if (getKey(response) == key)
            {
                const auto commandMessage = ximu3::XIMU3_command_message_parse(response.c_str());

                return CommandMessage
                {
                    commandMessage.json,
                    commandMessage.key,
                    commandMessage.value,
                    std::strlen(commandMessage.error) > 0 ? std::optional<std::string>(commandMessage.error) : std::nullopt,
                };
            }
        }

        return {};
    }
};
