#pragma once

#include "../CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

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
           const bool resizable_ = false,
           const std::optional<juce::Colour>& tag_ = {});

    ~Dialog() override;

    void resized() override;

    juce::Rectangle<int> getContentBounds(bool noMargins = false) const noexcept;

    bool isResizable() const;

    const juce::String icon;

    const std::optional<juce::Colour> tag;

    std::function<bool()> okCallback;

protected:
    static constexpr int margin = 15;
    static constexpr int dialogWidth = 520;
    static constexpr int columnWidth = 115;
    static constexpr int iconButtonWidth = 40;

    int calculateHeight(const int numberOfRows) const;

    void setOkButton(const bool valid, const juce::String& buttonText = "");

    void setCancelButton(const bool valid, const juce::String& buttonText = "");

private:
    juce::TextButton okButton { "OK" };
    juce::TextButton cancelButton { "Cancel" };

    juce::Component* const bottomLeftComponent;
    const int bottomLeftComponentWidth;

    const bool resizable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dialog)
};

class DialogQueue : private juce::DeletedAtShutdown
{
public:
    static DialogQueue& getSingleton()
    {
        static auto* singleton = new DialogQueue();
        return *singleton;
    }

    Dialog* getActive();

    void pushFront(std::unique_ptr<Dialog> content, std::function<bool()> okCallback = nullptr);

    void pushBack(std::unique_ptr<Dialog> content, std::function<bool()> okCallback = nullptr);

    void pop();

private:
    std::unique_ptr<juce::DialogWindow> active;
    std::list<std::unique_ptr<Dialog>> queue;
};
