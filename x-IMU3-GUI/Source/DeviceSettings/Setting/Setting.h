#pragma once

#include "../../CustomLookAndFeel.h"
#include "../../Widgets/Icon.h"
#include "../../Widgets/SimpleLabel.h"
#include "../DeviceSettingsIDs.h"
#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

class DeviceSettingsItem;

class Setting : public juce::Component,
                private juce::ValueTree::Listener
{
public:
    enum class Status
    {
        normal,
        modified,
        readFailed,
        writeFailed,
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
    Icon modifiedIcon { BinaryData::modify_svg, "Modified but Not Written to Device" };
    Icon readFailedIcon { BinaryData::warning_orange_svg, "Unable to Read from Device" };
    Icon writeFailedIcon { BinaryData::warning_orange_svg, "Unable to Write to Device" };
    juce::Rectangle<int> valueBounds;
    juce::WeakReference<DeviceSettingsItem> parentIfExpandable;

    void updateIcon();

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Setting)
};
