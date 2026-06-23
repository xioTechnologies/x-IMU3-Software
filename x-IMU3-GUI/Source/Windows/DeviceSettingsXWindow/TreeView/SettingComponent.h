#pragma once

#include "Widgets/SimpleLabel.h"
#include "../Values/SettingValue.h"

class SettingComponent : public juce::Component {
public:
    static constexpr int rowMargin = 4;

    SettingComponent(SettingX &setting_) : setting(setting_) {
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(statusLabel);
        addAndMakeVisible(value.get());

        refresh();
    }

    void resized() override {
        auto bounds = getLocalBounds().reduced(0, rowMargin / 2);

        statusLabel.setBounds(bounds.removeFromRight(75));

        if (const auto *const treeview = findParentComponentOfClass<juce::TreeView>()) {
            auto valueBounds = bounds.removeFromRight(juce::jmax(treeview->getWidth() / 3, treeview->getWidth() - 270));
            valueBounds.removeFromRight(2);
            valueBounds.reduce(0, juce::roundToInt(rowMargin * 0.5));

            value->setBounds(valueBounds);
        }

        nameLabel.setBounds(bounds);
    }

    void refresh() {
        switch (setting.status) {
            case SettingX::Status::unknown:
                statusLabel.setText("Unknown");
                break;

            case SettingX::Status::noResponse:
                statusLabel.setText("No Response");
                break;

            case SettingX::Status::errorResponse:
                statusLabel.setText("Error Response");
                break;

            case SettingX::Status::invalidResponse:
                statusLabel.setText("Invalid Response");
                break;

            case SettingX::Status::confirmed:
                statusLabel.setText("Confirmed");

                value->refresh();
                break;
        }
    }

private:
    SettingX &setting;
    SimpleLabel nameLabel{setting.name};
    const std::unique_ptr<SettingValue> value = SettingValue::create(setting);
    SimpleLabel statusLabel;
};
