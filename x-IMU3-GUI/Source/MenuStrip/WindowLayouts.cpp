#include "WindowLayouts.h"

bool WindowLayouts::exists(const juce::String& name) const
{
    return directory.findChildFiles(juce::File::findFiles, false, "*.xml").contains(directory.getChildFile(name + ".xml"));
}

void WindowLayouts::save(const juce::String& name, juce::ValueTree tree) const
{
    directory.createDirectory();
    directory.getChildFile(name + ".xml").replaceWithText(tree.toXmlString());
}

std::map<juce::String, juce::ValueTree> WindowLayouts::load() const
{
    std::map<juce::String, juce::ValueTree> result;
    for (const auto& file : directory.findChildFiles(juce::File::findFiles, false, "*.xml"))
    {
        result[file.getFileNameWithoutExtension()] = juce::ValueTree::fromXml(file.loadFileAsString());
    }
    return result;
}
