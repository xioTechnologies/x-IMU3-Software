#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting/SettingEnum.h"
#include "Setting/SettingText.h"
#include "Setting/SettingToggle.h"

class DeviceSettingsItem : public juce::TreeViewItem
{
public:
    DeviceSettingsItem(const juce::ValueTree settingsTree_, const std::vector<juce::ValueTree>& settingsFlattened, const juce::ValueTree enumsTree_, const bool& hideUnusedSettings_) : settingsTree(settingsTree_), enumsTree(enumsTree_), hideUnusedSettings(hideUnusedSettings_)
    {
        setLinesDrawnForSubItems(false);

        for (auto child : settingsTree)
        {
            addSubItem(new DeviceSettingsItem(child, settingsFlattened, enumsTree, hideUnusedSettings));
        }

        if (settingsTree.hasProperty(DeviceSettingsIds::hideKey))
        {
            for (const auto& setting : settingsFlattened)
            {
                if (setting[DeviceSettingsIds::key] == settingsTree[DeviceSettingsIds::hideKey])
                {
                    hideSetting = setting;
                    break;
                }
            }
        }

        setOpen(settingsTree[DeviceSettingsIds::expand]);
    }

    bool mightContainSubItems() override
    {
        return settingsTree.getNumChildren() > 0;
    }

    int getItemHeight() const override
    {
        if (settingsTree.getType() == DeviceSettingsIds::Margin)
        {
            return Setting::rowMargin / 2;
        }

        if (hideUnusedSettings && (isStatusSet(settingsTree) == false))
        {
            if (getParentItem() != nullptr && getParentItem()->getItemHeight() == 0)
            {
                return 0;
            }

            if (juce::StringArray::fromTokens(settingsTree[DeviceSettingsIds::hideValues].toString(), " ", {}).contains(hideSetting[DeviceSettingsIds::value].toString()))
            {
                return 0;
            }
        }

        return UILayout::textComponentHeight + Setting::rowMargin;
    }

    std::unique_ptr<juce::Component> createItemComponent() override
    {
        if (settingsTree.getType() == DeviceSettingsIds::Group)
        {
            return std::make_unique<Setting>(settingsTree, this);
        }

        if (settingsTree.getType() == DeviceSettingsIds::Margin)
        {
            return nullptr;
        }

        const auto type = settingsTree[DeviceSettingsIds::type];

        if (type == "string" || type == "number")
        {
            return std::make_unique<SettingText>(settingsTree);
        }

        if (type == "bool")
        {
            return std::make_unique<SettingToggle>(settingsTree);
        }

        return std::make_unique<SettingEnum>(settingsTree, enumsTree.getChildWithProperty(DeviceSettingsIds::name, type));
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
    const juce::ValueTree settingsTree;
    const juce::ValueTree enumsTree;
    juce::ValueTree hideSetting;
    const bool& hideUnusedSettings;

    static bool isStatusSet(juce::ValueTree tree_)
    {
        for (auto child : tree_)
        {
            if (isStatusSet(child))
            {
                return true;
            }
        }
        return (int) tree_[DeviceSettingsIds::status] != (int) Setting::Status::normal;
    }

    JUCE_DECLARE_WEAK_REFERENCEABLE(DeviceSettingsItem)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsItem)
};
