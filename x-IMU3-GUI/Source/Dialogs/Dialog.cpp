#include "Dialog.h"

Dialog::Dialog(const juce::String& icon_, const juce::String& dialogTitle, const juce::String& okButtonText, const juce::String& cancelButtonText, juce::Component* const bottomLeftComponent_, const int bottomLeftComponentWidth_, const bool resizable_, const std::optional<juce::Colour>& tag_)
        : juce::Component(dialogTitle),
          icon(icon_),
          tag(tag_),
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

    okButton.addShortcut(juce::KeyPress(juce::KeyPress::returnKey));
    if (cancelButton.isVisible())
    {
        cancelButton.addShortcut(juce::KeyPress(juce::KeyPress::escapeKey));
    }
    else
    {
        okButton.addShortcut(juce::KeyPress(juce::KeyPress::escapeKey));
    }

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

Dialog* DialogQueue::getActive()
{
    return active ? static_cast<Dialog*>(active->getContentComponent()) : nullptr;
}

void DialogQueue::pushFront(std::unique_ptr<Dialog> content, std::function<bool()> okCallback)
{
    if (okCallback != nullptr)
    {
        content->okCallback = okCallback;
    }

    queue.push_front(std::move(content));
    if (active == nullptr)
    {
        pop();
    }
}

void DialogQueue::pushBack(std::unique_ptr<Dialog> content, std::function<bool()> okCallback)
{
    if (okCallback != nullptr)
    {
        content->okCallback = okCallback;
    }

    queue.push_back(std::move(content));
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

    active = std::make_unique<juce::DialogWindow>(queue.front()->getName(), UIColours::backgroundLight, true, true);
    active->setContentOwned(queue.front().get(), true);
    active->setTitleBarHeight(Dialog::titleBarHeight);
    active->centreAroundComponent(nullptr, active->getWidth(), active->getHeight());
    active->setVisible(true);
    active->setTitleBarButtonsRequired(0, false);
    active->enterModalState(true);
    active->setResizable(queue.front()->isResizable(), queue.front()->isResizable());

    juce::Image iconImage(juce::Image::ARGB, 2 * 50, 2 * Dialog::titleBarHeight, true);
    juce::Graphics g(iconImage);
    const auto bounds = juce::Rectangle<float>((float) iconImage.getWidth(), (float) iconImage.getHeight()).withCentre(iconImage.getBounds().getCentre().toFloat());
    juce::Drawable::createFromSVG(*juce::XmlDocument::parse(queue.front()->icon))->drawWithin(g, bounds, juce::RectanglePlacement::centred, 1.0f);
    active->setIcon(iconImage);

    queue.front()->grabKeyboardFocus();
    queue.front().release();
    queue.erase(queue.begin());
}
