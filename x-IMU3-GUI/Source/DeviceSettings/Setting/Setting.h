#pragma once

#include <BinaryData.h>
#include "CustomLookAndFeel.h"
#include "DeviceSettings/DeviceSettingsIDs.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/SimpleLabel.h"

class DeviceSettingsItem;

class Setting : public juce::Component,
                private juce::ValueTree::Listener
{
public:
    enum class Status
    {
        normal,
        modified,
        warning,
    };

    static constexpr int rowMargin = 4;

    explicit Setting(const juce::ValueTree& tree_, DeviceSettingsItem* const parentIfExpandable_ = nullptr);

    ~Setting() override;

    void resized() override;

protected:
    juce::ValueTree tree;

    bool isReadOnly() const;

    const juce::Rectangle<int>& getValueBounds() const;

    std::optional<juce::var> getValue() const;

    void setValue(const juce::var& value);

    virtual void valueChanged()
    {
    }

private:
    SimpleLabel name;
    Icon modifiedIcon { BinaryData::modify_svg, {}};
    Icon warningIcon { BinaryData::warning_orange_svg, {}};
    juce::Rectangle<int> valueBounds;
    juce::WeakReference<DeviceSettingsItem> parentIfExpandable;

    void updateIcon();

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Setting)
};
