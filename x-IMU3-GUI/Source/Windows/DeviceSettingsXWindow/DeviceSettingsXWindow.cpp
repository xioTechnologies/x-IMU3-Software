#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsXWindow.h"
#include "Widgets/PopupMenuHeader.h"

DeviceSettingsXWindow::DeviceSettingsXWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu") {
    addAndMakeVisible(syncButton);
    addChildComponent(disabledOverlay);

    syncButton.onClick = [&] {
        disabledOverlay.setVisible(true);

        std::vector<std::string> commands;
        for (auto *const setting: treeView->getSettings()) {
            setting->clear();
            commands.push_back(setting->getRead());
        }

        connectionPanel.sendCommands(commands, this, [&](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            disabledOverlay.setVisible(false);

            for (size_t index = 0; index < responses.size(); index++) {
                treeView->getSettings()[index]->receive(responses[index]);
            }

            treeView->getRootGroup().refreshWarning();
        });
    };

    load(Schema::load(juce::ValueTree::fromXml(BinaryData::DeviceSettings_xml)));

    if (readFromValueTree().syncSettingsWhenWindowOpens) {
        syncButton.onClick();
    }

    setOpaque(true);
}

void DeviceSettingsXWindow::paint(juce::Graphics &g) {
    g.fillAll(UIColours::backgroundDark);
}

void DeviceSettingsXWindow::resized() {
    Window::resized();

    auto bounds = getContentBounds();
    disabledOverlay.setBounds(bounds);

    auto footer = bounds.removeFromBottom(25);
    syncButton.setBounds(footer.removeFromLeft(footer.getWidth() / 2));

    if (treeView != nullptr) {
        treeView->setBounds(bounds);
    }
}


void DeviceSettingsXWindow::writeToValueTree(const Settings &settings) {
    settingsTree.setProperty("hideUnusedSettings", settings.hideUnusedSettings, nullptr);
    settingsTree.setProperty("syncSettingsWhenWindowOpens", settings.syncSettingsWhenWindowOpens, nullptr);
}

DeviceSettingsXWindow::Settings DeviceSettingsXWindow::readFromValueTree() {
    Settings settings;
    settings.hideUnusedSettings = settingsTree.getProperty("hideUnusedSettings", settings.hideUnusedSettings);
    settings.syncSettingsWhenWindowOpens = settingsTree.getProperty("syncSettingsWhenWindowOpens", settings.syncSettingsWhenWindowOpens);
    return settings;
}

void DeviceSettingsXWindow::load(std::unique_ptr<Schema::Group> group) {
    treeView = std::make_unique<TreeView>(std::move(group), [this](Schema::Setting &setting, const std::string &command) {
        connectionPanel.sendCommands({command}, this, [this, &setting](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            setting.receive(responses.front());

            treeView->getRootGroup().refreshWarning();
        });
    }, [this] { return readFromValueTree().hideUnusedSettings; });
    addAndMakeVisible(*treeView);
    resized();
}

juce::PopupMenu DeviceSettingsXWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Sync Settings When Window Opens", true, readFromValueTree().syncSettingsWhenWindowOpens, [&] {
        auto settings = readFromValueTree();
        settings.syncSettingsWhenWindowOpens = !settings.syncSettingsWhenWindowOpens;
        writeToValueTree(settings);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("Hide Unused Settings", true, readFromValueTree().hideUnusedSettings, [&] {
        auto settings = readFromValueTree();
        settings.hideUnusedSettings = !settings.hideUnusedSettings;
        writeToValueTree(settings);

        treeView->getRootItem()->treeHasChanged();
    });

    // menu.addItem("Enumerate", [&] {
    //     juce::Thread::launch([this, self = SafePointer<juce::Component>(this)] {
    //         const auto group = std::make_shared<Schema::Group>();
    //
    //         while (true) {
    //             const auto response = connectionPanel.getConnection()->sendCommand({"{\"enumerate " + std::to_string(group->items.size()) + "\": null}"});
    //
    //             if (response.has_value() == false) {
    //                 return; // TODO: handle error
    //             }
    //
    //             if (response->error.has_value()) {
    //                 return; // TODO: handle error
    //             }
    //
    //             if (response->value == "null") {
    //                 break;
    //             }
    //
    //             const auto value = ximu3::CommandMessage::parse(response->value);
    //
    //             if (value.has_value() == false) {
    //                 return; // TODO: handle error
    //             }
    //
    //             // TODO: What if type is array?
    //             const auto type = typeFrom(value->valueType);
    //
    //             if (type.has_value() == false) {
    //                 return; // TODO: handle error
    //             }
    //
    //             auto setting = std::make_unique<Schema::Setting>();
    //             setting->key = value->key;
    //             setting->type = *type;
    //             setting->name = value->key;
    //
    //             group->items.push_back(std::move(setting));
    //         }
    //
    //         juce::MessageManager::callAsync([this, self, group] {
    //             if (self == nullptr) {
    //                 return;
    //             }
    //
    //             setGroup(std::move(*group));
    //         });
    //     });
    // });

    return menu;
}
