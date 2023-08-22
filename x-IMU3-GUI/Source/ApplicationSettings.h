#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ApplicationSettings : private juce::DeletedAtShutdown, private juce::ValueTree::Listener
{
    juce::ValueTree tree { "Settings" };

public:
    struct
    {
        juce::ValueTree tree;
        juce::CachedValue<bool> showOnStartup { tree, "showOnStartup", nullptr, true };
        juce::CachedValue<bool> usb { tree, "usb", nullptr, true };
        juce::CachedValue<bool> serial { tree, "serial", nullptr, true };
        juce::CachedValue<bool> tcp { tree, "tcp", nullptr, false };
        juce::CachedValue<bool> udp { tree, "udp", nullptr, true };
        juce::CachedValue<bool> bluetooth { tree, "bluetooth", nullptr, true };
    } searchForConnections { tree.getOrCreateChildWithName("SearchForConnections", nullptr) };

    struct
    {
        juce::ValueTree tree;
        juce::CachedValue<uint32_t> retries { tree, "retries", nullptr, 2 };
        juce::CachedValue<uint32_t> timeout { tree, "timeout", nullptr, 500 };
        juce::CachedValue<bool> closeSendingCommandDialogWhenComplete { tree, "closeSendingCommandDialogWhenComplete", nullptr, true };
    } commands { tree.getOrCreateChildWithName("Commands", nullptr) };

    struct
    {
        juce::ValueTree tree;
        juce::CachedValue<bool> readSettingsWhenWindowOpens { tree, "readSettingsWhenWindowOpens", nullptr, true };
        juce::CachedValue<bool> hideUnusedSettings { tree, "hideUnusedSettings", nullptr, true };
    } deviceSettings { tree.getOrCreateChildWithName("DeviceSettings", nullptr) };

    static ApplicationSettings& getSingleton()
    {
        static auto* singleton = new ApplicationSettings();
        return *singleton;
    }

    static juce::File getDirectory()
    {
        return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile(juce::JUCEApplication::getInstance()->getApplicationName());
    }

    juce::ValueTree& getTree()
    {
        return tree;
    }

    void restoreDefault()
    {
        file.deleteFile();
        load();
    }

private:
    const juce::File file = getDirectory().getChildFile("Settings.xml");

    ApplicationSettings()
    {
        load();
    }

    void load()
    {
        tree.removeListener(this);

        auto rootTree = juce::ValueTree::fromXml(file.loadFileAsString());
        searchForConnections.tree.copyPropertiesFrom(rootTree.getOrCreateChildWithName(searchForConnections.tree.getType(), nullptr), nullptr);
        commands.tree.copyPropertiesFrom(rootTree.getOrCreateChildWithName(commands.tree.getType(), nullptr), nullptr);
        deviceSettings.tree.copyPropertiesFrom(rootTree.getOrCreateChildWithName(deviceSettings.tree.getType(), nullptr), nullptr);

        tree.addListener(this);
    }

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        file.create();
        file.replaceWithText(tree.toXmlString());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApplicationSettings)
};

template<>
struct juce::VariantConverter<uint32_t>
{
    static uint32_t fromVar(const juce::var& value)
    {
        return (uint32_t) (int) value;
    }

    static juce::var toVar(uint32_t value)
    {
        return (int) value;
    }
};
