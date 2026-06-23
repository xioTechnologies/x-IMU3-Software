#pragma once

#include "../Schema.h"
#include "SettingItem.h"
#include "GroupItemComponent.h"

class GroupItem : public TreeViewItem {
public:
    GroupItem(Schema::Group &group_, const Schema::Group &rootGroup, const std::function<void(Schema::Setting &setting, const std::string &command)> &write, std::function<bool()> hideUnusedSettings_)
        : TreeViewItem(group_, rootGroup.find(group_.dependsOnKey), group_.dependsOnValues, hideUnusedSettings_),
          group(group_) {
        setLinesDrawnForSubItems(false);

        for (const auto &item_: group.items) {
            if (auto *const subGroup = dynamic_cast<Schema::Group*>(item_.get())) {
                addSubItem(new GroupItem(*subGroup, rootGroup, write, hideUnusedSettings_));
                continue;
            }

            if (auto *const setting = dynamic_cast<Schema::Setting*>(item_.get())) {
                addSubItem(new SettingItem(*setting, rootGroup.find(setting->dependsOnKey), write, hideUnusedSettings_));
            }
        }

        setOpen(group.expand);
    }

    bool mightContainSubItems() override {
        return true;
    }

    void itemOpennessChanged(bool) override {
        if (component) {
            component->refresh();
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

    void refresh() override {
        TreeViewItem::refresh();

        for (int index = 0; index < getNumSubItems(); index++) {
            if (auto *const item_ = dynamic_cast<TreeViewItem *>(getSubItem(index))) {
                item_->refresh();
            }
        }

        if (component) {
            component->refresh();
        }
    }

    void expandOrCollapseAll(const bool expand) {
        if (getParentItem() != nullptr) {
            setOpen(expand);
        }

        for (int index = 0; index < getNumSubItems(); index++) {
            if (auto *const item_ = dynamic_cast<GroupItem *>(getSubItem(index))) {
                item_->expandOrCollapseAll(expand);
            }
        }
    }

private:
    Schema::Group &group;
    juce::Component::SafePointer<GroupItemComponent> component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupItem)
};
