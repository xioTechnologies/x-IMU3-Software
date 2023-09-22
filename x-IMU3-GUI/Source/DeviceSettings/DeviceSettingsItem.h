#pragma once

#include "ApplicationSettings.h"
#include "Convert.h"
#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting/SettingEnum.h"
#include "Setting/SettingText.h"
#include "Setting/SettingToggle.h"
#include "CommandMessage.h"

class DeviceSettingsItem : public juce::TreeViewItem
{
public:
    DeviceSettingsItem(const juce::ValueTree& tree_, const std::map<juce::String, juce::ValueTree>& settings) : tree(tree_)
    {
        setLinesDrawnForSubItems(false);

        for (auto setting : tree)
        {
            addSubItem(new DeviceSettingsItem(setting, settings));
        }

        if (tree.hasProperty(DeviceSettingsIDs::hideKey))
        {
            for (const auto& setting : settings)
            {
                if (setting.second[DeviceSettingsIDs::key] == tree[DeviceSettingsIDs::hideKey])
                {
                    hideSetting = setting.second;
                    break;
                }
            }
        }
    }

    bool mightContainSubItems() override
    {
        return tree.getNumChildren() > 0;
    }

    int getItemHeight() const override
    {
        if (ApplicationSettings::getSingleton().deviceSettings.hideUnusedSettings && (isStatusSet(tree) == false))
        {
            if (getParentItem() != nullptr && getParentItem()->getItemHeight() == 0)
            {
                return 0;
            }

            if (juce::StringArray::fromTokens(tree[DeviceSettingsIDs::hideValues].toString(), " ", {}).contains(hideSetting[DeviceSettingsIDs::value].toString()))
            {
                return 0;
            }
        }

        return UILayout::textComponentHeight + Setting::rowMargin;
    }

    std::unique_ptr<juce::Component> createItemComponent() override
    {
        if (tree.getType() == DeviceSettingsIDs::Group)
        {
            return std::make_unique<Setting>(tree, this);
        }

        const auto type = tree[DeviceSettingsIDs::type];

        if (type == "string" || type == "number")
        {
            return std::make_unique<SettingText>(tree);
        }

        if (type == "bool")
        {
            return std::make_unique<SettingToggle>(tree);
        }

        return std::make_unique<SettingEnum>(tree, enums.getChildWithProperty(DeviceSettingsIDs::name, type));
    }

    void itemOpennessChanged(bool) override
    {
        if (onOpennessChanged != nullptr)
        {
            onOpennessChanged();
        }
    }

    std::function<void()> onOpennessChanged;

private:
    const juce::ValueTree tree;
    inline static const juce::ValueTree enums = juce::ValueTree::fromXml(BinaryData::DeviceSettingsEnums_xml);
    juce::ValueTree hideSetting;

    static bool isStatusSet(juce::ValueTree tree_)
    {
        for (auto child : tree_)
        {
            if (isStatusSet(child))
            {
                return true;
            }
        }
        return (int) tree_[DeviceSettingsIDs::status] != (int) Setting::Status::normal;
    }

    JUCE_DECLARE_WEAK_REFERENCEABLE(DeviceSettingsItem)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsItem)
};
