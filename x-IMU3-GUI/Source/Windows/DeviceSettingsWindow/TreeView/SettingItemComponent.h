#pragma once

#include "../Values/BooleanValue.h"
#include "../Values/EnumerationValue.h"
#include "../Values/RawValue.h"
#include "../Values/StringValue.h"
#include "../Values/Value.h"
#include "ItemComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

class SettingItemComponent final : public ItemComponent {
public:
    SettingItemComponent(SettingsSchema::Setting &setting, const std::function<void(SettingsSchema::Setting &setting, const std::string &command)> &write)
        : ItemComponent(setting, setting.name, std::invoke([&]() -> std::unique_ptr<juce::Component> {
              switch (setting.type) {
                  case SettingsSchema::Setting::Type::string:
                      return std::make_unique<StringValue>(setting, write);

                  case SettingsSchema::Setting::Type::number:
                  case SettingsSchema::Setting::Type::raw:
                      return std::make_unique<RawValue>(setting, write);

                  case SettingsSchema::Setting::Type::boolean:
                      return std::make_unique<BooleanValue>(setting, write);

                  case SettingsSchema::Setting::Type::enumeration:
                      return std::make_unique<EnumerationValue>(setting, write);
              }

              return {};
          })) {
        refresh();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItemComponent)
};
