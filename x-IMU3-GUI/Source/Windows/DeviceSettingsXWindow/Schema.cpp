#include "Schema.h"

Schema::Setting::Setting(const juce::ValueTree &tree, const juce::ValueTree &enums) {
    key = tree["key"].toString().toStdString();
    type = typeFrom(tree["type"].toString());
    if (type == Type::enumeration) {
        for (const auto &enumerator: enums.getChildWithProperty("name", tree["type"])) {
            enumeration.push_back({enumerator["value"], enumerator["name"].toString().toStdString()});
        }
    }
    readOnly = tree["readOnly"];
    name = tree["name"].toString().toStdString();
    emptyString = tree["emptyString"].toString().toStdString();
    dependsOnKey = tree["dependsOnKey"].toString().toStdString();
    dependsOnValues = vectorFrom(tree["dependsOnValues"]);
}

void Schema::Setting::clear() {
    sendChangeMessage();

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
    sendChangeMessage();

    if (response.has_value() == false) {
        status = Status::noResponse;
        return;
    }

    if (response->key != key) {
        return; // TODO: Delete this?
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

    if (false) {
        // TODO: Check map
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

Schema::Setting::Type Schema::Setting::typeFrom(const juce::String &type) {
    if (type == "string") {
        return Type::string;
    }

    if (type == "number") {
        return Type::number;
    }

    if (type == "bool") {
        return Type::boolean;
    }

    return Type::enumeration;
}

Schema::Group::Group(const juce::ValueTree &tree, const juce::ValueTree &enums) {
    name = tree["name"].toString().toStdString();
    expand = tree.getProperty("expand", true);
    dependsOnKey = tree["dependsOnKey"].toString().toStdString();
    dependsOnValues = vectorFrom(tree["dependsOnValues"]);

    for (auto child: tree) {
        if (child.getType().toString() == "Group") {
            items.push_back(std::make_unique<Group>(child, enums));
            continue;
        }

        if (child.getType().toString() == "Setting") {
            items.push_back(std::make_unique<Setting>(child, enums));
        }
    }
}

void Schema::Group::refreshWarning() {
    bool warning_ = false;

    for (const auto &item: items) {
        if (const auto *const group = std::get_if<std::unique_ptr<Group> >(&item)) {
            group->get()->refreshWarning();
            warning_ = warning_ || group->get()->warning;
            continue;
        }

        if (const auto *const setting = std::get_if<std::unique_ptr<Setting> >(&item)) {
            switch (setting->get()->status) {
                case Setting::Status::noResponse:
                case Setting::Status::errorResponse:
                case Setting::Status::invalidResponse:
                    warning_ = true;

                case Setting::Status::unknown:
                case Setting::Status::confirmed:
                    break;
            }
        }
    }

    if (warning != warning_) {
        warning = warning_;
        sendChangeMessage();
    }
}

std::vector<Schema::Setting *> Schema::Group::flatten() const {
    std::vector<Setting *> settings;

    for (const auto &item: items) {
        if (const auto *const group = std::get_if<std::unique_ptr<Group> >(&item)) {
            const auto flattened = group->get()->flatten();
            settings.insert(settings.end(), flattened.begin(), flattened.end());
            continue;
        }

        if (const auto *const setting = std::get_if<std::unique_ptr<Setting> >(&item)) {
            settings.push_back(setting->get());
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

std::unique_ptr<Schema::Group> Schema::load(const juce::ValueTree &tree) {
    return std::make_unique<Group>(tree.getChildWithName("Settings"), tree.getChildWithName("Enums"));
}

std::vector<std::string> Schema::vectorFrom(const juce::String &string) {
    std::vector<std::string> vector;

    for (const auto &string_: juce::StringArray::fromTokens(string, " ", {})) {
        vector.push_back(string_.toStdString());
    }

    return vector;
}
