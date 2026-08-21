#pragma once

#include "../Schema.h"
#include "SettingItem.h"
#include "GroupItemComponent.h"

class GroupItem : public TreeViewItem {
public:
    GroupItem(Schema::Group &group_, const Schema::Group &rootGroup, const std::function<void(Schema::Setting &setting, const std::string &command)> &write, std::function<bool()> hideUnusedSettings_)
        : TreeViewItem(rootGroup.find(group_.dependsOnKey), group_.dependsOnValues, hideUnusedSettings_),
          group(group_) {
        setLinesDrawnForSubItems(false);

        for (const auto &item: group.items) {
            if (const auto *const subGroup = std::get_if<std::unique_ptr<Schema::Group> >(&item)) {
                addSubItem(new GroupItem(*subGroup->get(), rootGroup, write, hideUnusedSettings_));
                continue;
            }

            if (const auto *const setting = std::get_if<std::unique_ptr<Schema::Setting> >(&item)) {
                addSubItem(new SettingItem(**setting, rootGroup.find((*setting)->dependsOnKey), write, hideUnusedSettings_));
            }
        }

        setOpen(group.expand);
    }

    bool mightContainSubItems() override {
        return true;
    }

    void itemOpennessChanged(bool) override {
        if (component) {
            component->refreshWarning();
        }
    }

    std::unique_ptr<juce::Component> createItemComponent() override {
        auto component_ = std::make_unique<GroupItemComponent>(group, *this);
        component = component_.get();
        return component_;
    }

    void itemClicked(const juce::MouseEvent &) override {
        setOpen(isOpen() == false);
    }

    void itemDoubleClicked(const juce::MouseEvent &) override {
    }

private:
    Schema::Group &group;
    juce::Component::SafePointer<GroupItemComponent> component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupItem)
};
