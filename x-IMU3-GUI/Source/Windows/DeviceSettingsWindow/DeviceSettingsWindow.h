#pragma once

#include "../Window.h"
#include "ApplicationSettings.h"
#include "InfoComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Schema/Schema.h"
#include "SettingsSchema.h"
#include "TreeView/TreeView.h"
#include "Widgets/DisabledOverlay.h"

class DeviceSettingsWindow final : public Window, private juce::AsyncUpdater {
public:
    DeviceSettingsWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_, juce::ThreadPool &threadPool_);

    ~DeviceSettingsWindow() override;

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    struct CommandSequenceItem {
        std::string key;
        bool handleError;
    };

    juce::ThreadPool &threadPool;

    juce::String currentSchema;
    juce::String autoSchemaName;

    std::unique_ptr<juce::Component> content;
    IconButton syncButton{BinaryData::sync_svg, "Sync with Device", nullptr, false};
    IconButton backupButton{BinaryData::download_svg, "Backup Settings"};
    IconButton restoreButton{BinaryData::upload_svg, "Restore Settings"};
    IconButton defaultsButton{BinaryData::default_svg, "Restore Defaults"};
    DisabledOverlay disabledOverlay;

    std::unique_ptr<juce::FileChooser> fileChooser;

    const juce::File deviceSettingsDirectory = ApplicationSettings::getDirectory().getChildFile("Device Settings");

    std::function<void(ximu3::XIMU3_PingResponse)> pingCallback;
    uint64_t pingCallbackId;

    bool syncWhenWindowOpens() const;

    bool hideUnusedSettings() const;

    void setSchema(const juce::String &schema);

    void showInfo(const juce::String &text, const std::optional<juce::String> &detail = {});

    void showSchema(std::unique_ptr<SettingsSchema::Group> group);

    void sendCommandSequence(std::vector<CommandSequenceItem> sequence, std::function<void()> epilogue);

    juce::PopupMenu getMenu() override;

    void valueTreePropertyChanged(juce::ValueTree &, const juce::Identifier &property) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsWindow)
};
