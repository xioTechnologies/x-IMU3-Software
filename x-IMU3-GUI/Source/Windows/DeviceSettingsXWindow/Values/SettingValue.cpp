#include "BooleanValue.h"
#include "EnumValue.h"
#include "NumberTextValue.h"
#include "SettingValue.h"
#include "StringTextValue.h"

std::unique_ptr<SettingValue> SettingValue::create(DeviceSettingX &setting) {
    switch (setting.type) {
        case DeviceSettingX::Type::string:
            return std::make_unique<StringTextValue>(setting);

        case DeviceSettingX::Type::number:
            if (setting.numberEnum.empty()) {
                return std::make_unique<NumberTextValue>(setting);
            }

            return std::make_unique<EnumValue>(setting);

        case DeviceSettingX::Type::boolean:
            return std::make_unique<BooleanValue>(setting);
    }

    return {}; // avoid compiler warning
}
