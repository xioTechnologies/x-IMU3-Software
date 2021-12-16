#pragma once

#include "../CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting/SettingEnum.h"
#include "Setting/SettingText.h"
#include "Setting/SettingToggle.h"

class DeviceSettingsItem : public juce::TreeViewItem
{
public:
    explicit DeviceSettingsItem(const juce::ValueTree& tree_) : tree(tree_)
    {
        setLinesDrawnForSubItems(false);

        for (auto setting : tree)
        {
            addSubItem(new DeviceSettingsItem(setting));
        }
    }

    bool mightContainSubItems() override
    {
        return tree.getNumChildren() > 0;
    }

    int getItemHeight() const override
    {
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

        return std::make_unique<SettingEnum>(tree, enums.getChildWithProperty(DeviceSettingsIDs::typeName, type));
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
    const juce::ValueTree enums = juce::ValueTree::fromXml(BinaryData::DeviceSettingsTypes_xml);

    JUCE_DECLARE_WEAK_REFERENCEABLE(DeviceSettingsItem)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsItem)
};
