#include "SerialAccessoryTextEditor.h"

SerialAccessoryTextEditor::SerialAccessoryTextEditor() {
    setEscapeAndReturnKeysConsumed(true);

    loadPrevious();
}

bool SerialAccessoryTextEditor::keyPressed(const juce::KeyPress &key) {
    if (key == juce::KeyPress::upKey || key == juce::KeyPress::downKey) {
        index += (key == juce::KeyPress::upKey) ? 1 : -1;
        index = juce::jlimit(0, previousData.getNumChildren() - 1, index);
        setText(previousData.getChild(index)["data"]);
        return true;
    }

    if (key == juce::KeyPress::returnKey) {
        for (const auto data: previousData) {
            if (data["data"] == getText()) {
                previousData.removeChild(data, nullptr);
                break;
            }
        }

        while (previousData.getNumChildren() >= 50) {
            previousData.removeChild(previousData.getChild(previousData.getNumChildren() - 1), nullptr);
        }

        previousData.addChild({"Data", {{"data", getText()}}}, 0, nullptr);
        file.replaceWithText(previousData.toXmlString());

        loadPrevious();
    }

    return CustomTextEditor::keyPressed(key);
}

void SerialAccessoryTextEditor::loadPrevious() {
    index = 0;

    previousData = juce::ValueTree::fromXml(file.loadFileAsString());
    if (previousData.isValid() == false) {
        previousData = juce::ValueTree("SerialAccessoryData");
        previousData.appendChild({"Data", {{"data", R"(Use escape sequences "\x00" to "\xFF" to send any byte value)"}}}, nullptr);
    }

    setText(previousData.getChild(0)["data"]);
}
