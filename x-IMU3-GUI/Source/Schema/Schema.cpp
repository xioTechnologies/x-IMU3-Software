#include "ApplicationSettings.h"
#include <BinaryData.h>
#include "Schema.h"
#include "SchemaIds.h"

std::vector<Schema> Schema::getAll() {
    std::vector<Schema> schemas;

    for (const auto &file: getDirectory().findChildFiles(juce::File::findFiles, false, "*.xml")) {
        const auto tree = juce::ValueTree::fromXml(file.loadFileAsString());

        Schema schema;
        schema.fileName = file.getFileName();
        schema.name = tree.getProperty(SchemaIds::name, schema.fileName);
        schema.commands = tree.getChildWithName(SchemaIds::Commands);
        schema.settings = tree.getChildWithName(SchemaIds::Settings);
        schemas.push_back(schema);
    }

    std::sort(schemas.begin(), schemas.end(), [](const auto &schemaA, const auto &schemaB) {
        return schemaA.name.compareNatural(schemaB.name) < 0;
    });

    return schemas;
}

std::optional<Schema> Schema::get(const juce::String &fileName) {
    for (auto &schema: getAll()) {
        if (schema.fileName.equalsIgnoreCase(fileName)) {
            return schema;
        }
    }

    return {};
}

std::optional<Schema> Schema::find(juce::String model) {
    if (model.isEmpty()) {
        model = "x-IMU3"; // legacy firmware will return empty model string
    }

    for (auto &schema: getAll()) {
        if (schema.name.equalsIgnoreCase(model)) {
            return schema;
        }
    }

    for (auto &schema: getAll()) {
        if (schema.fileName.containsIgnoreCase(model)) {
            return schema;
        }
    }

    return {};
}

std::unique_ptr<juce::FileChooser> Schema::add(std::function<void(const juce::File &)> callback) {
    auto fileChooser = std::make_unique<juce::FileChooser>("Select Schema", juce::File(), "*.xml");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [&, callback](const auto &chooser) {
        if (chooser.getResult() == juce::File()) {
            return;
        }

        const auto destination = getDirectory().getChildFile(chooser.getResult().getFileName());

        if (destination != chooser.getResult()) {
            std::ignore = getDirectory().createDirectory();
            std::ignore = chooser.getResult().copyFileTo(destination);
        }

        callback(destination);
    });

    return fileChooser;
}

void Schema::copyDefaultSchemas() {
    const std::vector<std::pair<juce::String, juce::MemoryBlock> > schemas
    {
        {"x-IMU3 Schema.xml", {BinaryData::xIMU3_Schema_xml, (size_t) BinaryData::xIMU3_Schema_xmlSize}},
        {"x-IMU4 Schema.xml", {BinaryData::xIMU4_Schema_xml, (size_t) BinaryData::xIMU4_Schema_xmlSize}},
    };

    std::ignore = getDirectory().createDirectory();

    for (const auto &[fileName, data]: schemas) {
        std::ignore = getDirectory().getChildFile(fileName).replaceWithData(data.getData(), data.getSize());
    }
}

juce::File Schema::getDirectory() {
    return ApplicationSettings::getDirectory().getChildFile("Schemas");
}
