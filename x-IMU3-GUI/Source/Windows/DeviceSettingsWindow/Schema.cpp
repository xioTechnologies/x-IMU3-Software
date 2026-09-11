#include <algorithm>
#include "ApplicationSettings.h"
#include <BinaryData.h>
#include "Schema.h"
#include "SchemaIds.h"

Schema::Item::Item(const std::string &name_) : name(name_) {
}

Schema::Item::Item(const juce::ValueTree &tree) {
    name = tree[SchemaIds::name].toString().toStdString();
    dependsOnKey = tree[SchemaIds::dependsOnKey].toString().toStdString();
    for (const auto &string_: juce::StringArray::fromTokens(tree[SchemaIds::dependsOnValues].toString(), " ", {})) {
        dependsOnValues.push_back(string_.toStdString());
    }
}

Schema::Setting::Setting(const juce::ValueTree &tree) : Item(tree) {
    key = tree[SchemaIds::key].toString().toStdString();
    if (tree[SchemaIds::type] == "string") {
        type = Type::string;
    } else if (tree[SchemaIds::type] == "number") {
        type = Type::number;
    } else if (tree[SchemaIds::type] == "bool") {
        type = Type::boolean;
    } else if (tree[SchemaIds::type] == "enum") {
        type = Type::enumeration;
        for (const auto &enumerator: tree) {
            enumeration.push_back({enumerator[SchemaIds::value], enumerator[SchemaIds::name].toString().toStdString()});
        }
    } else {
        type = Type::raw;
    }
    readOnly = tree[SchemaIds::readOnly];
    emptyString = tree[SchemaIds::emptyString].toString().toStdString();
}

Schema::Setting::Setting(const std::string &key_, const Type type_)
    : Item(key_),
      key(key_),
      type(type_) {
}

std::optional<std::string> Schema::Setting::getError() const {
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

std::string Schema::Setting::getReadCommand() const {
    return "{\"" + key + "\":null}";
}

std::string Schema::Setting::getWriteCommand(const std::string &value_) const {
    if (readOnly) {
        return getReadCommand();
    }

    const std::string command = "{\"" + key + "\":" + value_ + "}";

    if (ximu3::CommandMessage::parse(command).has_value() == false) {
        return getReadCommand();
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
            if (std::ranges::any_of(enumeration, [=](const auto &enumerator) { return std::to_string(enumerator.first) == response->value; }) == false) {
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

Schema::Group::Group(const juce::ValueTree &tree) : Item(tree) {
    expand = tree.getProperty(SchemaIds::expand, true);

    for (auto child: tree) {
        if (child.hasType(SchemaIds::Group)) {
            items.push_back(std::make_unique<Group>(child));
            continue;
        }

        if (child.hasType(SchemaIds::Setting)) {
            items.push_back(std::make_unique<Setting>(child));
        }
    }
}

std::optional<std::string> Schema::Group::getError() const {
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
    return std::make_unique<Group>(tree.getChildWithName(SchemaIds::Settings));
}

std::unique_ptr<Schema::Group> Schema::loadSchema(std::shared_ptr<ximu3::Connection> connection) {
    std::vector<std::unique_ptr<Item> > settings;

    for (int index = 0; ; index++) {
        const auto command = "{\"enumerate " + std::to_string(index) + "\":null}";
        const auto response = connection->sendCommand({command});

        if (response.has_value() == false) {
            throw std::runtime_error("No response to " + command);
        }

        if (response->error.has_value()) {
            throw std::runtime_error("Error response to " + command + ": " + *response->error);
        }

        if (response->value == "null") {
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

static const std::vector<std::pair<juce::String, juce::MemoryBlock> > builtInSchemas
{
    {"x-IMU3 Schema.xml", {BinaryData::xIMU3_Schema_xml, (size_t) BinaryData::xIMU3_Schema_xmlSize}},
    {"x-IMU4 Schema.xml", {BinaryData::xIMU4_Schema_xml, (size_t) BinaryData::xIMU4_Schema_xmlSize}},
};

juce::File Schema::getSchemasDirectory() {
    return ApplicationSettings::getDirectory().getChildFile("Schemas");
}

void Schema::copyBuiltInSchemas() {
    std::ignore = getSchemasDirectory().createDirectory();

    for (const auto &[fileName, data]: builtInSchemas) {
        std::ignore = getSchemasDirectory().getChildFile(fileName).replaceWithData(data.getData(), data.getSize());
    }
}

std::vector<juce::File> Schema::getSchemaFiles() {
    std::vector<juce::File> files;

    for (const auto &file: getSchemasDirectory().findChildFiles(juce::File::findFiles, false, "*.xml")) {
        files.push_back(file);
    }

    std::sort(files.begin(), files.end(), [](const auto &fileA, const auto &fileB) {
        return fileA.getFileName().compareNatural(fileB.getFileName()) < 0;
    });

    return files;
}

juce::File Schema::findSchemaFileMatching(const juce::String &model) {
    const auto files = getSchemaFiles();

    if (model.isNotEmpty()) {
        for (const auto &file: files) {
            if (getSchemaModel(file).equalsIgnoreCase(model)) {
                return file;
            }
        }

        for (const auto &file: files) {
            if (getSchemaName(file).equalsIgnoreCase(model)) {
                return file;
            }
        }

        for (const auto &file: files) {
            if (file.getFileName().containsIgnoreCase(model)) {
                return file;
            }
        }
    }

    return getFallbackSchemaFile();
}

juce::File Schema::getFallbackSchemaFile() {
    return getSchemasDirectory().getChildFile(builtInSchemas.front().first);
}

juce::String Schema::getSchemaName(const juce::File &file) {
    if (const auto name = juce::ValueTree::fromXml(file.loadFileAsString())[SchemaIds::name].toString(); name.isNotEmpty()) {
        return name;
    }

    return file.getFileName();
}

juce::String Schema::getSchemaModel(const juce::File &file) {
    return juce::ValueTree::fromXml(file.loadFileAsString())[SchemaIds::model].toString();
}
