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
            DialogLauncher::launchDialog(nullptr);
        }
    };

    cancelButton.onClick = []
    {
        DialogLauncher::launchDialog(nullptr);
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

std::unique_ptr<DialogLauncher> DialogLauncher::launchedDialog = nullptr;

void DialogLauncher::launchDialog(std::unique_ptr<Dialog> content, std::function<bool()> okCallback)
{
    launchedDialog.reset();

    if (content != nullptr)
    {
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

DialogLauncher::DialogLauncher(std::unique_ptr<Dialog> content, std::function<bool()> okCallback)
        : juce::DialogWindow(content->getName(), UIColours::backgroundLight, true, true)
{
    if (okCallback != nullptr)
    {
        content->okCallback = std::move(okCallback);
    }

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
