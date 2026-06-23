#include <algorithm>
#include "ApplicationSettings.h"
#include "SettingsIds.h"
#include "SettingsSchema.h"

SettingsSchema::Item::Item(const std::string &name_) : name(name_) {
}

SettingsSchema::Item::Item(const juce::ValueTree &tree) {
    name = tree[SettingsIds::name].toString().toStdString();
    dependsOnKey = tree[SettingsIds::dependsOnKey].toString().toStdString();
    for (const auto &string_: juce::StringArray::fromTokens(tree[SettingsIds::dependsOnValues].toString(), " ", {})) {
        dependsOnValues.push_back(string_.toStdString());
    }
}

SettingsSchema::Setting::Setting(const juce::ValueTree &tree) : Item(tree) {
    key = tree[SettingsIds::key].toString().toStdString();
    if (tree[SettingsIds::type] == "string") {
        type = Type::string;
    } else if (tree[SettingsIds::type] == "number") {
        type = Type::number;
    } else if (tree[SettingsIds::type] == "bool") {
        type = Type::boolean;
    } else if (tree[SettingsIds::type] == "enum") {
        type = Type::enumeration;
        for (const auto &enumerator: tree) {
            enumeration.push_back({enumerator[SettingsIds::value], enumerator[SettingsIds::name].toString().toStdString()});
        }
    } else {
        type = Type::raw;
    }
    readOnly = tree[SettingsIds::readOnly];
    emptyString = tree[SettingsIds::emptyString].toString().toStdString();
}

SettingsSchema::Setting::Setting(const std::string &key_, const Type type_)
    : Item(key_),
      key(key_),
      type(type_) {
}

std::optional<std::string> SettingsSchema::Setting::getError() const {
    switch (status) {
        case Status::unknown:
        case Status::confirmed:
            return {};

        case Status::noResponse:
            return "No Response";

        case Status::errorResponse:
            return std::string("Error Response: ") + error;

        case Status::invalidResponse:
            return "Invalid Response";
    }

    return {}; // fix compiler warning
}

void SettingsSchema::Setting::clear() {
    value = {};
    error = {};
    status = Status::unknown;
}

std::string SettingsSchema::Setting::getReadCommand() const {
    return "{\"" + key + "\":null}";
}

std::string SettingsSchema::Setting::getWriteCommand(const std::string &value_) const {
    if (readOnly) {
        return getReadCommand();
    }

    const std::string command = "{\"" + key + "\":" + value_ + "}";

    if (ximu3::CommandMessage::parse(command).has_value() == false) {
        return getReadCommand();
    }

    return command;
}

void SettingsSchema::Setting::receive(const std::optional<ximu3::CommandMessage> &response) {
    if (response.has_value() == false) {
        status = Status::noResponse;
        return;
    }

    if (response->error.has_value()) {
        error = response->error.value();
        status = Status::errorResponse;
        return;
    }

    switch (type) {
        case Type::string:
            if (response->valueType != ximu3::XIMU3_JsonTypeString) {
                status = Status::invalidResponse;
                return;
            }
            break;

        case Type::number:
            if (response->valueType != ximu3::XIMU3_JsonTypeNumber) {
                status = Status::invalidResponse;
                return;
            }
            break;

        case Type::boolean:
            if (response->valueType != ximu3::XIMU3_JsonTypeBoolean) {
                status = Status::invalidResponse;
                return;
            }
            break;

        case Type::enumeration:
            if (response->valueType != ximu3::XIMU3_JsonTypeNumber) {
                status = Status::invalidResponse;
                return;
            }
            if (std::ranges::any_of(enumeration, [&](const auto &enumerator) { return std::to_string(enumerator.first) == response->value; }) == false) {
                status = Status::invalidResponse;
                return;
            }
            break;

        case Type::raw:
            break;
    }

    value = response->value;
    status = Status::confirmed;
}

std::string SettingsSchema::Setting::getStringValue() const {
    try {
        return value.substr(1, value.size() - 2);
    } catch (...) {
        return {};
    }
}

SettingsSchema::Group::Group(std::vector<std::unique_ptr<Item> > settings) : Item("") {
    items = std::move(settings);
}

SettingsSchema::Group::Group(const juce::ValueTree &tree) : Item(tree) {
    expand = tree.getProperty(SettingsIds::expand, true);

    for (auto child: tree) {
        if (child.hasType(SettingsIds::Group)) {
            items.push_back(std::make_unique<Group>(child));
            continue;
        }

        if (child.hasType(SettingsIds::Setting)) {
            items.push_back(std::make_unique<Setting>(child));
        }
    }
}

std::optional<std::string> SettingsSchema::Group::getError() const {
    for (const auto &item: items) {
        if (auto *const group = dynamic_cast<const Group *>(item.get())) {
            if (auto error = group->getError()) {
                return error;
            }
            continue;
        }

        if (const auto *const setting = dynamic_cast<const Setting *>(item.get())) {
            switch (setting->status) {
                case Setting::Status::unknown:
                case Setting::Status::confirmed:
                    break;

                case Setting::Status::noResponse:
                case Setting::Status::errorResponse:
                case Setting::Status::invalidResponse:
                    return "Sync Failed";
            }
        }
    }

    return {};
}

std::vector<SettingsSchema::Setting *> SettingsSchema::Group::flatten() const {
    std::vector<Setting *> settings;

    for (const auto &item: items) {
        if (auto *const group = dynamic_cast<Group *>(item.get())) {
            const auto flattened = group->flatten();
            settings.insert(settings.end(), flattened.begin(), flattened.end());
            continue;
        }

        if (auto *const setting = dynamic_cast<Setting *>(item.get())) {
            settings.push_back(setting);
        }
    }

    return settings;
}

SettingsSchema::Setting *SettingsSchema::Group::find(const std::string &key) const {
    for (auto *const setting: flatten()) {
        if (setting->key == key) {
            return setting;
        }
    }

    return nullptr;
}

std::unique_ptr<SettingsSchema::Group> SettingsSchema::loadSchema(const juce::ValueTree &tree) {
    return std::make_unique<Group>(tree);
}

std::unique_ptr<SettingsSchema::Group> SettingsSchema::loadSchema(std::shared_ptr<ximu3::Connection> connection) {
    std::vector<std::unique_ptr<Item> > settings;

    for (int index = 0; ; index++) {
        const auto command = "{\"enumerate " + std::to_string(index) + "\":null}";
        const auto response = connection->sendCommand({command}, ApplicationSettings::getSingleton().commands.retries, ApplicationSettings::getSingleton().commands.timeout);

        if (response.has_value() == false) {
            throw std::runtime_error("No response to " + command);
        }

        if (response->error.has_value()) {
            throw std::runtime_error("Error response to " + command + ": " + *response->error);
        }

        if (response->valueType == ximu3::XIMU3_JsonTypeNull) {
            break;
        }

        const auto value = ximu3::CommandMessage::parse(response->value);

        if (value.has_value() == false) {
            throw std::runtime_error("Invalid response to " + command);
        }

        switch (value->valueType) {
            case ximu3::XIMU3_JsonTypeString:
                settings.push_back(std::make_unique<Setting>(value->key, Setting::Type::string));
                break;

            case ximu3::XIMU3_JsonTypeNumber:
                settings.push_back(std::make_unique<Setting>(value->key, Setting::Type::number));
                break;

            case ximu3::XIMU3_JsonTypeBoolean:
                settings.push_back(std::make_unique<Setting>(value->key, Setting::Type::boolean));
                break;

            case ximu3::XIMU3_JsonTypeNull:
                throw std::runtime_error("Invalid response to " + command);

            case ximu3::XIMU3_JsonTypeObject:
            case ximu3::XIMU3_JsonTypeArray:
                settings.push_back(std::make_unique<Setting>(value->key, Setting::Type::raw));
                break;
        }
    }

    return std::make_unique<Group>(std::move(settings));
}
