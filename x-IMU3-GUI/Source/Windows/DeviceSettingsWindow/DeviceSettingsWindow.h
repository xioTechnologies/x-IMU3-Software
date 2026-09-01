#pragma once

#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Schema.h"
#include "TreeView/TreeView.h"
#include "Widgets/DisabledOverlay.h"
#include "ApplicationSettings.h"

class DeviceSettingsWindow final : public Window, private juce::AsyncUpdater {
public:
    DeviceSettingsWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_, juce::ThreadPool &threadPool_);

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    enum class SchemaType {
        enumerate,
        ximu3,
        custom,
    };

    juce::ThreadPool &threadPool;

    std::unique_ptr<TreeView> treeView;
    IconButton syncButton{BinaryData::sync_svg, "Sync with Device", nullptr, false};
    IconButton backupButton{BinaryData::download_svg, "Backup Settings"};
    IconButton restoreButton{BinaryData::upload_svg, "Restore Settings"};
    IconButton defaultsButton{BinaryData::default_svg, "Restore Defaults"};
    DisabledOverlay disabledOverlay;

    juce::AttributedString enumerationError;

    std::unique_ptr<juce::FileChooser> fileChooser;

    const juce::File schemasDirectory = ApplicationSettings::getDirectory().getChildFile("Schemas");
    const juce::File deviceSettingsDirectory = ApplicationSettings::getDirectory().getChildFile("Device Settings");

    static SchemaType schemaTypeFrom(const int schema);

    void syncSettings();

    SchemaType getSchemaType() const;

    juce::File getCustomSchema() const;

    void setCustomSchema(const juce::File &customSchema);

    bool syncWhenWindowOpens();

    bool hideUnusedSettings() const;

    void loadSchema(std::unique_ptr<Schema::Group> group);

    void sendCommand(const std::string &key, const bool silent, std::function<void()> callback);

    juce::PopupMenu getMenu() override;

    void valueTreePropertyChanged(juce::ValueTree &, const juce::Identifier &property) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsWindow)
};
