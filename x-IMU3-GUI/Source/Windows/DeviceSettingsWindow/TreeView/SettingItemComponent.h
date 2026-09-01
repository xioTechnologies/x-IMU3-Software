#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Values/BooleanValue.h"
#include "../Values/EnumerationValue.h"
#include "../Values/RawValue.h"
#include "../Values/StringValue.h"
#include "../Values/Value.h"
#include "ItemComponent.h"

class SettingItemComponent final : public ItemComponent {
public:
    SettingItemComponent(Schema::Setting &setting_, const std::function<void(Schema::Setting &setting, const std::string &command)> &write)
        : ItemComponent(setting_, setting_.name, std::invoke([&]() -> std::unique_ptr<juce::Component> {
              switch (setting_.type) {
                  case Schema::Setting::Type::raw:
                  case Schema::Setting::Type::number:
                      return std::make_unique<RawValue>(setting_, write);

                  case Schema::Setting::Type::string:
                      return std::make_unique<StringValue>(setting_, write);

                  case Schema::Setting::Type::boolean:
                      return std::make_unique<BooleanValue>(setting_, write);

                  case Schema::Setting::Type::enumeration:
                      return std::make_unique<EnumerationValue>(setting_, write);
              }

              return {};
          })),
          setting(setting_) {
        refresh();
    }

private:
    Schema::Setting &setting;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItemComponent)
};
