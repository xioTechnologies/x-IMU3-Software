#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ApplicationSettings : private juce::DeletedAtShutdown
{
    juce::ValueTree tree { "Settings" };

public:
    juce::CachedValue<bool> searchForConnectionsOnStartup { tree, "searchForConnectionsOnStartup", nullptr, true };
    juce::CachedValue<bool> checkFirmwareVersionOnStartup { tree, "checkFirmwareVersionOnStartup", nullptr, true };
    juce::CachedValue<bool> checkDeviceTimeOnStartup { tree, "checkDeviceTimeOnStartup", nullptr, true };
    juce::CachedValue<bool> searchUsb { tree, "searchUsb", nullptr, true };
    juce::CachedValue<bool> searchSerial { tree, "searchSerial", nullptr, true };
    juce::CachedValue<bool> searchTcp { tree, "searchTcp", nullptr, false };
    juce::CachedValue<bool> searchUdp { tree, "searchUdp", nullptr, true };
    juce::CachedValue<bool> searchBluetooth { tree, "searchBluetooth", nullptr, true };
    juce::CachedValue<uint32_t> retries { tree, "retries", nullptr, 2 };
    juce::CachedValue<uint32_t> timeout { tree, "timeout", nullptr, 500 };
    juce::CachedValue<bool> hideUnusedDeviceSettings { tree, "hideUnusedDeviceSettings", nullptr, true };
    juce::CachedValue<bool> showApplicationErrors { tree, "showApplicationErrors", nullptr, true };
    juce::CachedValue<bool> showNotificationAndErrorMessages { tree, "showNotificationAndErrorMessages", nullptr, true };

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

    void save()
    {
        file.create();
        file.replaceWithText(tree.toXmlString());
    }

    void load()
    {
        tree.copyPropertiesFrom(juce::ValueTree::fromXml(file.loadFileAsString()), nullptr);
    }

    void restoreDefault()
    {
        file.deleteFile();
        load();
    }

private:
    ApplicationSettings()
    {
        load();
    }

    const juce::File file = getDirectory().getChildFile("Settings.xml");

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
