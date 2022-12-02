#pragma once

#include "../CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/DialogButton.h"

class IconButton;

class Dialog : public juce::Component
{
public:
    static constexpr int titleBarHeight = 38;

    Dialog(const juce::String& icon_,
           const juce::String& dialogTitle,
           const juce::String& okButtonText = "OK",
           const juce::String& cancelButtonText = "Cancel",
           juce::Component* const bottomLeftComponent_ = nullptr,
           const int bottomLeftComponentWidth_ = 0,
           const bool resizable_ = false);

    ~Dialog() override;

    void resized() override;

    juce::Rectangle<int> getContentBounds(bool noMargins = false) const noexcept;

    bool isResizable() const;

    const juce::String icon;

    std::function<bool()> okCallback;

protected:
    static constexpr int margin = 15;
    static constexpr int dialogWidth = 520;
    static constexpr int columnWidth = 115;

    int calculateHeight(const int numberOfRows) const;

    void setOkButton(const bool valid, const juce::String& buttonText = "");

    void setCancelButton(const bool valid, const juce::String& buttonText = "");

private:
    DialogButton okButton { "OK" };
    DialogButton cancelButton { "Cancel" };

    juce::Component* const bottomLeftComponent;
    const int bottomLeftComponentWidth;

    const bool resizable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dialog)
};

class DialogLauncher : public juce::DialogWindow
{
public:
    static void launchDialog(std::unique_ptr<Dialog> content, std::function<bool()> okCallback = nullptr);

    static Dialog* getLaunchedDialog();

    void closeButtonPressed() override;

    bool escapeKeyPressed() override;

private:
    static std::unique_ptr<DialogLauncher> launchedDialog;

    DialogLauncher(std::unique_ptr<Dialog> content, std::function<bool()> okCallback);

    void dismiss();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DialogLauncher)
};
