#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Values/BooleanValue.h"
#include "../Values/EnumerationValue.h"
#include "../Values/NumberValue.h"
#include "../Values/StringValue.h"
#include "ItemComponent.h"

class SettingItemComponent final : public ItemComponent,
                                   private juce::ChangeListener {
public:
    SettingItemComponent(Schema::Setting &setting_, const std::function<void(Schema::Setting &setting, const std::string &command)> &write)
        : ItemComponent(setting_.name, std::invoke([&]() -> std::unique_ptr<juce::Component> {
              switch (setting_.type) {
                  case Schema::Setting::Type::string:
                      return std::make_unique<StringValue>(setting_, write);

                  case Schema::Setting::Type::number:
                      return std::make_unique<NumberValue>(setting_, write);

                  case Schema::Setting::Type::enumeration:
                      return std::make_unique<EnumerationValue>(setting_, write);

                  case Schema::Setting::Type::boolean:
                      return std::make_unique<BooleanValue>(setting_, write);
              }

              return {};
          })),
          setting(setting_) {
        setting.addChangeListener(this);
        changeListenerCallback({});
    }

    ~SettingItemComponent() override {
        setting.removeChangeListener(this);
    }

private:
    Schema::Setting &setting;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        switch (setting.status) {
            case Schema::Setting::Status::unknown:
            case Schema::Setting::Status::confirmed:
                showWarning(false, {});
                return;

            case Schema::Setting::Status::noResponse:
                showWarning(true, "No Response");
                break;

            case Schema::Setting::Status::errorResponse:
                showWarning(true, "Error Response: " + setting.error);
                break;

            case Schema::Setting::Status::invalidResponse:
                showWarning(true, "Invalid Response");
                break;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItemComponent)
};
