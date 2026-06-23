#pragma once

#include "SettingX.h"
#include "Margin.h"

struct Group
{
    juce::String name;

    std::vector<std::variant<Group, SettingX, Margin>> items;

    std::vector<SettingX*> flatten()
    {
        std::vector<SettingX*> vector;

        for (auto &item: items) {
            if (auto* const group = std::get_if<Group>(&item)) {
                const auto flattened = group->flatten();
                vector.insert(vector.end(), flattened.begin(), flattened.end());
                continue;
            }

            if (auto* const setting = std::get_if<SettingX>(&item)) {
                vector.push_back(setting);
            }
        }

        return vector;
    }
};
