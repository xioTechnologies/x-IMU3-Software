#include "ApplicationSettings.h"
#include <BinaryData.h>
#include "Dialogs/MessageDialog.h"
#include "Models.h"

std::vector<juce::File> Models::getAll() {
    std::vector<juce::File> models;

    for (const auto &file: getDirectory().findChildFiles(juce::File::findFiles, false, "*.obj")) {
        models.push_back(file);
    }

    std::sort(models.begin(), models.end(), [](const auto &modelA, const auto &modelB) {
        return modelA.getFileNameWithoutExtension().compareNatural(modelB.getFileNameWithoutExtension()) < 0;
    });

    return models;
}

std::optional<juce::File> Models::get(const juce::String &fileName) {
    for (const auto &model: getAll()) {
        if (model.getFileName().equalsIgnoreCase(fileName)) {
            return model;
        }
    }

    return {};
}

std::optional<juce::File> Models::find(juce::String model) {
    if (model.isEmpty()) {
        model = "x-IMU3"; // legacy firmware will return empty model string
    }

    for (const auto &file: getAll()) {
        if (file.getFileNameWithoutExtension().equalsIgnoreCase(model)) {
            return file;
        }
    }

    for (const auto &file: getAll()) {
        if (file.getFileNameWithoutExtension().containsIgnoreCase(model)) {
            return file;
        }
    }

    return {};
}

std::unique_ptr<juce::FileChooser> Models::add(std::function<void(const juce::File &)> callback) {
    auto fileChooser = std::make_unique<juce::FileChooser>("Select 3D Model", juce::File(), "*.obj");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [callback](const auto &chooser) {
        const auto source = chooser.getResult();

        if (source == juce::File()) {
            return;
        }

        const auto destination = getDirectory().getChildFile(source.getFileName());

        const auto addModel = [callback, source, destination] {
            if (source != destination) {
                const auto sourceMtl = source.withFileExtension("mtl");
                const auto destinationMtl = destination.withFileExtension("mtl");

                std::ignore = getDirectory().createDirectory();
                std::ignore = source.copyFileTo(destination);
                std::ignore = destinationMtl.deleteFile(); // do not keep materials of replaced model
                if (sourceMtl.existsAsFile()) {
                    std::ignore = sourceMtl.copyFileTo(destinationMtl);
                }
            }

            callback(destination);
        };

        if (destination.existsAsFile() && source != destination) {
            DialogQueue::getSingleton().pushFront(std::make_unique<ConfirmReplaceDialog>(destination.getFileName()), [addModel] {
                addModel();
                return true;
            });
            return;
        }

        addModel();
    });

    return fileChooser;
}

void Models::copyDefaultModels() {
    const std::vector<std::pair<const char *, int> > models
    {
        {BinaryData::xIMU3_Board_zip, BinaryData::xIMU3_Board_zipSize},
        {BinaryData::xIMU3_zip, BinaryData::xIMU3_zipSize},
    };

    std::ignore = getDirectory().createDirectory();

    for (const auto &[data, size]: models) {
        juce::MemoryInputStream stream(data, (size_t) size, false);
        std::ignore = juce::ZipFile (stream).uncompressTo(getDirectory());
    }
}

juce::File Models::getDirectory() {
    return ApplicationSettings::getDirectory().getChildFile("3D Models");
}
