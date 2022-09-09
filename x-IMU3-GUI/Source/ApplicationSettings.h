#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ApplicationSettings : private juce::DeletedAtShutdown, private juce::ValueTree::Listener
{
    juce::ValueTree tree { "Settings" };

public:
    juce::CachedValue<bool> showSearchForConnectionsOnStartup { tree, "showSearchForConnectionsOnStartup", nullptr, true };
    juce::CachedValue<bool> searchUsb { tree, "searchUsb", nullptr, true };
    juce::CachedValue<bool> searchSerial { tree, "searchSerial", nullptr, true };
    juce::CachedValue<bool> searchTcp { tree, "searchTcp", nullptr, false };
    juce::CachedValue<bool> searchUdp { tree, "searchUdp", nullptr, true };
    juce::CachedValue<bool> searchBluetooth { tree, "searchBluetooth", nullptr, true };
    juce::CachedValue<uint32_t> retries { tree, "retries", nullptr, 2 };
    juce::CachedValue<uint32_t> timeout { tree, "timeout", nullptr, 500 };
    juce::CachedValue<bool> hideUnusedDeviceSettings { tree, "hideUnusedDeviceSettings", nullptr, true };
    juce::CachedValue<bool> closeSendingCommandDialogWhenComplete { tree, "closeSendingCommandDialogWhenComplete", nullptr, false };

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
        tree.copyPropertiesFrom(juce::ValueTree::fromXml(file.loadFileAsString()), nullptr);
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
