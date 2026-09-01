#include "SchemaIds.h"
#include "Schema.h"
#include <algorithm>

Schema::Item::Item(const std::string &name_) : name(name_) {
}

Schema::Item::Item(const juce::ValueTree &tree) {
    name = tree[SchemaIds::name].toString().toStdString();
    dependsOnKey = tree[SchemaIds::dependsOnKey].toString().toStdString();
    for (const auto &string_: juce::StringArray::fromTokens(tree[SchemaIds::dependsOnValues].toString(), " ", {})) {
        dependsOnValues.push_back(string_.toStdString());
    }
}

Schema::Setting::Setting(const juce::ValueTree &tree, const juce::ValueTree &enums) : Item(tree) {
    key = tree[SchemaIds::key].toString().toStdString();
    if (tree[SchemaIds::type] == "string") {
        type = Type::string;
    } else if (tree[SchemaIds::type] == "number") {
        type = Type::number;
    } else if (tree[SchemaIds::type] == "bool") {
        type = Type::boolean;
    } else {
        type = Type::enumeration;
        for (const auto &enumerator: enums.getChildWithProperty(SchemaIds::name, tree[SchemaIds::type])) {
            enumeration.push_back({enumerator[SchemaIds::value], enumerator[SchemaIds::name].toString().toStdString()});
        }
    }
    readOnly = tree[SchemaIds::readOnly];
    emptyString = tree[SchemaIds::emptyString].toString().toStdString();
}

Schema::Setting::Setting(const std::string &key_, const Type type_)
    : Item(key_),
      key(key_),
      type(type_) {
}

std::optional<std::string> Schema::Setting::getWarning() const {
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

void Schema::Setting::clear() {
    value = {};
    error = {};
    status = Status::unknown;
}

std::string Schema::Setting::getRead() const {
    return "{\"" + key + "\":null}";
}

std::string Schema::Setting::getWrite(const std::string &value_) const {
    if (readOnly) {
        return getRead();
    }

    const std::string command = "{\"" + key + "\":" + value_ + "}";

    if (ximu3::CommandMessage::parse(command).has_value() == false) {
        return getRead();
    }

    return command;
}

void Schema::Setting::receive(const std::optional<ximu3::CommandMessage> &response) {
    if (response.has_value() == false) {
        status = Status::noResponse;
        return;
    }

    if (response->error.has_value()) {
        error = response->error.value();
        status = Status::errorResponse;
        return;
    }

    if (jsonTypeFrom(type) != response->valueType) {
        status = Status::invalidResponse;
        return;
    }

    if (type == Type::enumeration && std::ranges::any_of(enumeration, [=](const auto &enumerator) { return std::to_string(enumerator.first) == response->value; }) == false) {
        status = Status::invalidResponse;
        return;
    }

    value = response->value;
    status = Status::confirmed;
}

ximu3::XIMU3_JsonType Schema::Setting::jsonTypeFrom(const Type type_) {
    switch (type_) {
        case Type::string:
            return ximu3::XIMU3_JsonTypeString;
        case Type::number:
        case Type::enumeration:
            return ximu3::XIMU3_JsonTypeNumber;
        case Type::boolean:
            return ximu3::XIMU3_JsonTypeBoolean;
    }

    return {}; // avoid compiler warning
}

std::string Schema::Setting::getStringValue() const {
    try {
        return value.substr(1, value.size() - 2);
    } catch (...) {
        return {};
    }
}

Schema::Group::Group(std::vector<std::unique_ptr<Item> > settings) : Item("") {
    items = std::move(settings);
}

Schema::Group::Group(const juce::ValueTree &tree, const juce::ValueTree &enums) : Item(tree) {
    expand = tree.getProperty(SchemaIds::expand, true);

    for (auto child: tree) {
        if (child.hasType(SchemaIds::Group)) {
            items.push_back(std::make_unique<Group>(child, enums));
            continue;
        }

        if (child.hasType(SchemaIds::Setting)) {
            items.push_back(std::make_unique<Setting>(child, enums));
        }
    }
}

std::optional<std::string> Schema::Group::getWarning() const {
    for (const auto &item: items) {
        if (auto *const group = dynamic_cast<const Group *>(item.get())) {
            if (auto warning = group->getWarning()) {
                return warning;
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

std::vector<Schema::Setting *> Schema::Group::flatten() const {
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

Schema::Setting *Schema::Group::find(const std::string &key) const {
    for (auto *const setting: flatten()) {
        if (setting->key == key) {
            return setting;
        }
    }

    return nullptr;
}

std::unique_ptr<Schema::Group> Schema::loadSchema(const juce::ValueTree &tree) {
    return std::make_unique<Group>(tree.getChildWithName(SchemaIds::Settings), tree.getChildWithName(SchemaIds::Enums));
}

std::unique_ptr<Schema::Group> Schema::loadSchema(std::shared_ptr<ximu3::Connection> connection) {
    std::vector<std::unique_ptr<Item> > settings;

    for (int index = 0; ; index++) {
        const auto response = connection->sendCommand({"{\"enumerate " + std::to_string(index) + "\": null}"});

        if (response.has_value() == false) {
            return {}; // TODO: No response to {"enumerate 0":null}
        }

        if (response->error.has_value()) {
            return {}; // TODO: Error response to {"enumerate 0":null}: Unknown command
        }

        if (response->value == "null") {
            break;
        }

        const auto value = ximu3::CommandMessage::parse(response->value);

        if (value.has_value() == false) {
            return {}; // TODO: Invalid response to {"enumerate 0":null}
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
            case ximu3::XIMU3_JsonTypeObject:
            case ximu3::XIMU3_JsonTypeArray:
                break; // TODO: Invalid response to {"enumerate 0":null}
        }
    }

    return std::make_unique<Group>(std::move(settings));
}
