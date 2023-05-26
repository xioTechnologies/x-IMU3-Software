#include "Dialog.h"

Dialog::Dialog(const juce::String& icon_, const juce::String& dialogTitle, const juce::String& okButtonText, const juce::String& cancelButtonText, juce::Component* const bottomLeftComponent_, const int bottomLeftComponentWidth_, const bool resizable_)
        : juce::Component(dialogTitle),
          icon(icon_),
          bottomLeftComponent(bottomLeftComponent_),
          bottomLeftComponentWidth(bottomLeftComponentWidth_),
          resizable(resizable_)
{
    auto initButton = [this](auto& button, auto visible, const auto& buttonText, auto toggleState)
    {
        addChildComponent(&button);
        button.setToggleState(toggleState, juce::dontSendNotification);
        button.setWantsKeyboardFocus(false);
        button.setVisible(visible);
        button.setButtonText(buttonText);
    };

    initButton(okButton, okButtonText.isNotEmpty(), okButtonText, true);
    initButton(cancelButton, cancelButtonText.isNotEmpty(), cancelButtonText, false);

    okButton.addShortcut(juce::KeyPress(juce::KeyPress::returnKey)); // Esc is already managed by parent juce::DialogWindow

    okButton.onClick = [this]
    {
        BailOutChecker thisDeletedChecker(this);

        if (okCallback != nullptr && okCallback() == false)
        {
            return;
        }

        if (thisDeletedChecker.shouldBailOut() == false)
        {
            DialogQueue::getSingleton().pop();
        }
    };

    cancelButton.onClick = []
    {
        DialogQueue::getSingleton().pop();
    };
}

Dialog::~Dialog()
{
}

void Dialog::resized()
{
    auto bounds = getLocalBounds().removeFromBottom(UILayout::textComponentHeight + margin).reduced(margin, 0).withTrimmedBottom(margin);

    okButton.changeWidthToFitText();
    cancelButton.changeWidthToFitText();

    okButton.setBounds(bounds.removeFromRight(okButton.getWidth()));

    if (cancelButton.isVisible())
    {
        bounds.removeFromRight(margin);
        cancelButton.setBounds(bounds.removeFromRight(cancelButton.getWidth()));
    }

    if (bottomLeftComponent)
    {
        bounds.removeFromRight(margin);
        bottomLeftComponent->setBounds(bounds.removeFromLeft(bottomLeftComponentWidth));
    }
}

juce::Rectangle<int> Dialog::getContentBounds(bool noMargins) const noexcept
{
    const auto bounds = getLocalBounds().withTrimmedBottom(UILayout::textComponentHeight + margin);
    if (noMargins)
    {
        return bounds.withTrimmedBottom(margin);
    }
    return bounds.reduced(margin);
}

bool Dialog::isResizable() const
{
    return resizable;
}

int Dialog::calculateHeight(const int numberOfRows) const
{
    // This "adjust" fixes a bug that the dialog height doesn't adjust after changing the title bar height
    static constexpr int adjust = titleBarHeight - 26; // 26 is juce default title bar height,
    return margin + (numberOfRows + 1) * (UILayout::textComponentHeight + margin) + adjust;
}

void Dialog::setOkButton(const bool valid, const juce::String& buttonText)
{
    okButton.setEnabled(valid);
    if (buttonText.isNotEmpty())
    {
        okButton.setButtonText(buttonText);
    }
}

void Dialog::setCancelButton(const bool valid, const juce::String& buttonText)
{
    cancelButton.setEnabled(valid);
    if (buttonText.isNotEmpty())
    {
        cancelButton.setButtonText(buttonText);
    }
}

DialogWindow::DialogWindow(std::unique_ptr<Dialog> content)
        : juce::DialogWindow(content->getName(), UIColours::backgroundLight, true, true)
{
    setContentOwned(content.get(), true);
    setTitleBarHeight(Dialog::titleBarHeight);
    centreAroundComponent(nullptr, getWidth(), getHeight());
    setVisible(true);
    setTitleBarButtonsRequired(0, false);

    enterModalState(true);

    content->grabKeyboardFocus();

    if (content->isResizable())
    {
        setResizable(true, true);
    }

    juce::Image iconImage(juce::Image::ARGB, 2 * 50, 2 * Dialog::titleBarHeight, true);
    juce::Graphics g(iconImage);
    const auto bounds = juce::Rectangle<float>((float) iconImage.getWidth(), (float) iconImage.getHeight()).withCentre(iconImage.getBounds().getCentre().toFloat());
    juce::Drawable::createFromSVG(*juce::XmlDocument::parse(content->icon))->drawWithin(g, bounds, juce::RectanglePlacement::centred, 1.0f);
    setIcon(iconImage);

    content.release();
}

void DialogWindow::closeButtonPressed()
{
    DialogQueue::getSingleton().pop();
}

bool DialogWindow::escapeKeyPressed()
{
    DialogQueue::getSingleton().pop();
    return true;
}

Dialog* DialogQueue::getActive()
{
    return active ? static_cast<Dialog*>(active->getContentComponent()) : nullptr;
}

void DialogQueue::push(std::unique_ptr<Dialog> content, std::function<bool()> okCallback)
{
    content->okCallback = okCallback;

    queue.push(std::move(content));
    if (active == nullptr)
    {
        pop();
    }
}

void DialogQueue::pop()
{
    active.reset();

    if (queue.empty())
    {
        return;
    }

    active = std::make_unique<DialogWindow>(std::move(queue.front()));
    queue.pop();
}
