#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CommandMessage
{
public:
    std::string json;
    juce::String key;
    juce::var value;

    CommandMessage(const juce::String& key_, const juce::var& value_)
            : json("{" + key_.quoted().toStdString() + ":" + juce::JSON::toString(value_).toStdString() + "}"),
              key(key_),
              value(value_)
    {
    }

    explicit CommandMessage(const std::string& json_) : json(json_)
    {
        const auto parsed = juce::JSON::parse(json);
        if (auto* object = parsed.getDynamicObject())
        {
            key = object->getProperties().getName(0).toString();
            value = object->getProperties().getValueAt(0);
        }
    }

    bool operator==(const CommandMessage& other) const
    {
        return key.isNotEmpty() && key == other.key;
    }

    operator const std::string&() const
    {
        return json;
    }
};
