#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/SimpleLabel.h"

class ItemComponent : public juce::Component {
public:
    static constexpr int rowMargin = 4;

    ItemComponent(const Schema::Item& item_, const juce::String& name, std::unique_ptr<juce::Component> value_) : item(item_), nameLabel(name), value(std::move(value_)) {
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(value.get());
        addChildComponent(warningIcon);
    }

    void resized() override {
        auto bounds = getLocalBounds().reduced(0, rowMargin / 2);

        warningIcon.setBounds(bounds.removeFromRight(25).reduced(5));

        if (const auto *const treeview = findParentComponentOfClass<juce::TreeView>()) {
            if (value) {
                auto valueBounds = bounds.removeFromRight(juce::jmax(treeview->getWidth() / 3, treeview->getWidth() - 270));
                valueBounds.removeFromRight(2);

                value->setBounds(valueBounds);
            }
        }

        nameLabel.setBounds(bounds);
    }

    void refresh() {
        if (auto *const value_ = dynamic_cast<Value *>(value.get())) {
            value_->refresh();
        }

        const auto error = item.getError();

        if (error.has_value() == false || hideWarningIcon()) {
            warningIcon.setVisible(false);
            return;
        }

        warningIcon.setVisible(true);
        warningIcon.setTooltip(*error);
    }

protected:
    virtual bool hideWarningIcon() const { return false; }

private:
    const Schema::Item& item;
    SimpleLabel nameLabel;
    std::unique_ptr<juce::Component> value;
    Icon warningIcon{BinaryData::warning_orange_svg, {}};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ItemComponent)
};
