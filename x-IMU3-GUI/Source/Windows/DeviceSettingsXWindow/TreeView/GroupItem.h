#pragma once

#include "../Schema.h"
#include "SettingItem.h"

class GroupItem : public juce::TreeViewItem,
                  private juce::ChangeListener {
public:
    GroupItem(const Schema::Group &group_, const Schema::Group &rootGroup, const std::function<void(Schema::Setting &setting, const std::string &command)> &write) : group(group_),
                                                                                                                                                                     dependsOn(rootGroup.find(group_.dependsOnKey)) {
        setLinesDrawnForSubItems(false);

        if (dependsOn != nullptr) {
            dependsOn->addChangeListener(this);
        }

        for (const auto &item: group.items) {
            if (const auto *const subGroup = std::get_if<Schema::Group>(&item)) {
                addSubItem(new GroupItem(*subGroup, rootGroup, write));
                continue;
            }

            if (const auto *const setting = std::get_if<std::unique_ptr<Schema::Setting> >(&item)) {
                addSubItem(new SettingItem(**setting, rootGroup.find((*setting)->dependsOnKey), write));
            }
        }

        setOpen(group.expand);
    }

    ~GroupItem() override {
        if (dependsOn != nullptr) {
            dependsOn->removeChangeListener(this);
        }
    }

    bool mightContainSubItems() override {
        return getNumSubItems() > 0;
    }

    std::unique_ptr<juce::Component> createItemComponent() override {
        return std::make_unique<SimpleLabel>(group.name);
    }

    int getItemHeight() const override {
        if (isHidden()) {
            return 0;
        }

        return UILayout::textComponentHeight + SettingItemComponent::rowMargin;
    }

private:
    const Schema::Group &group;
    Schema::Setting *const dependsOn;

    bool isHidden() const {
        if (const auto *const parent = getParentItem(); parent != nullptr && parent->getItemHeight() == 0) {
            return true;
        }

        if (dependsOn == nullptr || dependsOn->status != Schema::Setting::Status::confirmed) {
            return false;
        }

        return std::find(group.dependsOnValues.begin(), group.dependsOnValues.end(), dependsOn->value) != group.dependsOnValues.end();
    }

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        treeHasChanged();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupItem)
};
