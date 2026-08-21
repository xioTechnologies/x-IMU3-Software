#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/SimpleLabel.h"
#include "../Values/BooleanValue.h"
#include "../Values/EnumerationValue.h"
#include "../Values/NumberValue.h"
#include "../Values/StringValue.h"

class SettingItemComponent : public juce::Component,
                             private juce::ChangeListener {
public:
    static constexpr int rowMargin = 4;

    SettingItemComponent(Schema::Setting &setting_, const std::function<void(Schema::Setting &setting, const std::string &command)> &write) : setting(setting_) {
        addAndMakeVisible(nameLabel);
        addChildComponent(warningIcon);

        switch (setting.type) {
            case Schema::Setting::Type::string:
                value = std::make_unique<StringValue>(setting, write);
                break;

            case Schema::Setting::Type::number:
                value = std::make_unique<NumberValue>(setting, write);
                break;

            case Schema::Setting::Type::enumeration:
                value = std::make_unique<EnumerationValue>(setting, write);
                break;

            case Schema::Setting::Type::boolean:
                value = std::make_unique<BooleanValue>(setting, write);
                break;
        }

        addAndMakeVisible(*value);

        setting.addChangeListener(this);
        changeListenerCallback({});
    }

    ~SettingItemComponent() override {
        setting.removeChangeListener(this);
    }

    void resized() override {
        auto bounds = getLocalBounds().reduced(0, rowMargin / 2);

        warningIcon.setBounds(bounds.removeFromRight(25).reduced(5));

        if (const auto *const treeview = findParentComponentOfClass<juce::TreeView>()) {
            auto valueBounds = bounds.removeFromRight(juce::jmax(treeview->getWidth() / 3, treeview->getWidth() - 270));
            valueBounds.removeFromRight(2);

            value->setBounds(valueBounds);
        }

        nameLabel.setBounds(bounds);
    }

private:
    Schema::Setting &setting;
    SimpleLabel nameLabel{setting.name};
    std::unique_ptr<juce::Component> value;
    Icon warningIcon{BinaryData::warning_orange_svg, {}};

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        switch (setting.status) {
            case Schema::Setting::Status::unknown:
            case Schema::Setting::Status::confirmed:
                warningIcon.setVisible(false);
                return;

            case Schema::Setting::Status::noResponse:
                warningIcon.setTooltip("No Response");
                warningIcon.setVisible(true);
                break;

            case Schema::Setting::Status::errorResponse:
                warningIcon.setTooltip("Error Response: " + setting.error);
                warningIcon.setVisible(true);
                break;

            case Schema::Setting::Status::invalidResponse:
                warningIcon.setTooltip("Invalid Response");
                warningIcon.setVisible(true);
                break;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItemComponent)
};
