#pragma once

#include "Widgets/SimpleLabel.h"
#include "../Values/BooleanValue.h"
#include "../Values/EnumValue.h"
#include "../Values/NumberValue.h"
#include "../Values/Value.h"
#include "../Values/StringValue.h"

class SettingItemComponent : public juce::Component {
public:
    static constexpr int rowMargin = 4;

    SettingItemComponent(SettingX &setting_) : setting(setting_) {
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(statusLabel);

        switch (setting.type) {
            case SettingX::Type::string:
                value = std::make_unique<StringValue>(setting);
                break;

            case SettingX::Type::number:
                if (setting.numberEnum.empty()) {
                    value = std::make_unique<NumberValue>(setting);
                } else {
                    value = std::make_unique<EnumValue>(setting);
                }
                break;

            case SettingX::Type::boolean:
                value = std::make_unique<BooleanValue>(setting);
                break;
        }

        addAndMakeVisible(value.get());

        setting.onRefresh = [&, self = SafePointer<juce::Component>(this)] {
            if (self) {
                refresh();
            }
        };

        refresh();
    }

    void resized() override {
        auto bounds = getLocalBounds().reduced(0, rowMargin / 2);

        statusLabel.setBounds(bounds.removeFromRight(75));

        if (const auto *const treeview = findParentComponentOfClass<juce::TreeView>()) {
            auto valueBounds = bounds.removeFromRight(juce::jmax(treeview->getWidth() / 3, treeview->getWidth() - 270));
            valueBounds.removeFromRight(2);

            if (value) {
                value->setBounds(valueBounds);
            }
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

                if (value) {
                    value->refresh();
                }
                break;
        }
    }

private:
    SettingX &setting;
    SimpleLabel nameLabel{setting.name};
    std::unique_ptr<Value> value;
    SimpleLabel statusLabel;
};
