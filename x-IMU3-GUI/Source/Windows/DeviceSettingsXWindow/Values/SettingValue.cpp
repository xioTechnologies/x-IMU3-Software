#include "BooleanValue.h"
#include "EnumValue.h"
#include "NumberTextValue.h"
#include "SettingValue.h"
#include "StringTextValue.h"

std::unique_ptr<SettingValue> SettingValue::create(SettingX &setting) {
    switch (setting.type) {
        case SettingX::Type::string:
            return std::make_unique<StringTextValue>(setting);

        case SettingX::Type::number:
            if (setting.numberEnum.empty()) {
                return std::make_unique<NumberTextValue>(setting);
            }

            return std::make_unique<EnumValue>(setting);

        case SettingX::Type::boolean:
            return std::make_unique<BooleanValue>(setting);
    }

    return {}; // avoid compiler warning
}
