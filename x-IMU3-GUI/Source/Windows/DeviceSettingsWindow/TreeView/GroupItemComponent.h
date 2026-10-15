#pragma once

#include "../SettingsSchema.h"
#include "ItemComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

class GroupItemComponent final : public ItemComponent {
public:
    GroupItemComponent(SettingsSchema::Group &group_, juce::TreeViewItem &parent_)
        : ItemComponent(group_, group_.name, {}),
          parent(parent_) {
        setInterceptsMouseClicks(false, true);

        refresh();
    }

private:
    juce::TreeViewItem &parent;

    bool hideWarningIcon() const override {
        return parent.isOpen();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupItemComponent)
};
