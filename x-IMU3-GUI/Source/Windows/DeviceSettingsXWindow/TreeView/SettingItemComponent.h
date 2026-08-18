#pragma once

#include "Widgets/SimpleLabel.h"
#include "../Values/BooleanValue.h"
#include "../Values/EnumValue.h"
#include "../Values/NumberValue.h"
#include "../Values/StringValue.h"

class SettingItemComponent : public juce::Component,
                             private juce::ChangeListener {
public:
    static constexpr int rowMargin = 4;

    SettingItemComponent(Schema::Setting &setting_, const std::function<void(Schema::Setting &setting, const std::string &command)> &write) : setting(setting_) {
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(statusLabel);

        switch (setting.type) {
            case Schema::Setting::Type::string:
                value = std::make_unique<StringValue>(setting, write);
                break;

            case Schema::Setting::Type::number:
                value = std::make_unique<NumberValue>(setting, write);
                break;

            case Schema::Setting::Type::enumeration:
                value = std::make_unique<EnumValue>(setting, write);
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

        statusLabel.setBounds(bounds.removeFromRight(75));

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
    SimpleLabel statusLabel;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        switch (setting.status) {
            case Schema::Setting::Status::unknown:
                statusLabel.setText("Unknown");
                break;

            case Schema::Setting::Status::noResponse:
                statusLabel.setText("No Response");
                break;

            case Schema::Setting::Status::errorResponse:
                statusLabel.setText("Error Response");
                break;

            case Schema::Setting::Status::invalidResponse:
                statusLabel.setText("Invalid Response");
                break;

            case Schema::Setting::Status::confirmed:
                statusLabel.setText("Confirmed");
                break;
        }

        statusLabel.setTooltip(setting.error);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItemComponent)
};
