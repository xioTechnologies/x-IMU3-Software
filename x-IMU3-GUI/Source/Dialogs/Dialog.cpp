#include "Dialog.h"
#include "Widgets/IconButton.h"

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
        button.setClickingTogglesState(true);
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

        if (okCallback)
        {
            okCallback();
        }

        if (thisDeletedChecker.shouldBailOut() == false)
        {
            findParentComponentOfClass<juce::DialogWindow>()->closeButtonPressed();
        }
    };

    cancelButton.onClick = [this]
    {
        findParentComponentOfClass<juce::DialogWindow>()->closeButtonPressed();
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
        bottomLeftComponent->setBounds(bounds.removeFromLeft(bottomLeftComponentWidth));
    }
}

bool Dialog::keyPressed(const juce::KeyPress& key)
{
    if (key.getKeyCode() == juce::KeyPress::returnKey)
    {
        okButton.triggerClick();
        return true;
    }
    return false;
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

void Dialog::setValid(const bool valid)
{
    okButton.setEnabled(valid);
}

std::unique_ptr<DialogLauncher> DialogLauncher::launchedDialog = nullptr;

void DialogLauncher::launchDialog(std::unique_ptr<Dialog> content, std::function<void()> okCallback)
{
    launchedDialog.reset();
    launchedDialog.reset(new DialogLauncher(std::move(content), std::move(okCallback)));

    static const struct CleanupAtShutdown : juce::DeletedAtShutdown
    {
        ~CleanupAtShutdown() override
        {
            launchedDialog.reset();
        }
    } * cleanupAtShutdown = new CleanupAtShutdown();
    juce::ignoreUnused(cleanupAtShutdown);
}

Dialog* DialogLauncher::getLaunchedDialog()
{
    return launchedDialog ? static_cast<Dialog*>(launchedDialog->getContentComponent()) : nullptr;
}

void DialogLauncher::closeButtonPressed()
{
    dismiss();
}

bool DialogLauncher::escapeKeyPressed()
{
    dismiss();
    return true;
}

DialogLauncher::DialogLauncher(std::unique_ptr<Dialog> content, std::function<void()> okCallback)
        : juce::DialogWindow(content->getName(), UIColours::menuStrip, true, true)
{
    content->okCallback = std::move(okCallback);

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

void DialogLauncher::dismiss()
{
    exitModalState(0);
    launchedDialog.reset();
}
