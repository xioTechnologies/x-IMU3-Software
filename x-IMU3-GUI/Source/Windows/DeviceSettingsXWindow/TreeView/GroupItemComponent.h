#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ItemComponent.h"
#include "../Schema.h"

class GroupItemComponent final : public ItemComponent,
                                 private juce::ChangeListener {
public:
    GroupItemComponent(Schema::Group &group_, juce::TreeViewItem &parent_)
        : ItemComponent(group_.name, {}),
          group(group_),
          parent(parent_) {
        setInterceptsMouseClicks(false, false);

        group.addChangeListener(this);
        refreshWarning();
    }

    ~GroupItemComponent() override {
        group.removeChangeListener(this);
    }

    void refreshWarning() {
        showWarning(group.warning && parent.isOpen() == false, "Unable to Sync");
    }

private:
    Schema::Group &group;
    juce::TreeViewItem &parent;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        refreshWarning();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupItemComponent)
};
