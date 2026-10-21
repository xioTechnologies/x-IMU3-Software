#include "../CustomLookAndFeel.h"
#include "CommandProgressRow.h"

CommandProgressRow::CommandProgressRow(const juce::Colour &colourTag_, const juce::String &connection)
    : colourTag(colourTag_),
      connectionLabel(connection) {
    addAndMakeVisible(connectionLabel);
    addChildComponent(progressBar);
    addChildComponent(percentageLabel);
    addChildComponent(responseLabel);
    addAndMakeVisible(icon);
}

void CommandProgressRow::paint(juce::Graphics &g) {
    g.setColour(colourTag);
    g.fillRect(getLocalBounds().withWidth(UILayout::colourTagWidth));
}

void CommandProgressRow::resized() {
    auto bounds = getLocalBounds();

    bounds.removeFromLeft(UILayout::colourTagWidth + 5);

    icon.setBounds(bounds.removeFromRight(50));

    if (responseLabel.isVisible()) {
        responseLabel.setBounds(bounds.removeFromRight((int) std::ceil(responseLabel.getTextWidth())));
    } else {
        percentageLabel.setBounds(bounds.removeFromRight(40));
        progressBar.setBounds(bounds.removeFromRight(125).withSizeKeepingCentre(125, 4));
    }

    bounds.removeFromRight(10);
    connectionLabel.setBounds(bounds);
}

void CommandProgressRow::setInProgress(std::optional<int> percentage) {
    progressBar.setVisible(percentage.has_value());
    percentageLabel.setVisible(percentage.has_value());
    if (percentage) {
        progressBar.setProgress((float) *percentage / 100.0f);
        percentageLabel.setText(juce::String(*percentage) + "%");
    }

    responseLabel.setVisible(false);

    resized();

    setStatus(Status::inProgress);
}

void CommandProgressRow::setComplete(const juce::String &value) {
    showResponse(value, UIColours::success);
    setStatus(Status::complete);
}

void CommandProgressRow::setFailed(const juce::String &error) {
    showResponse(error, UIColours::warning);
    setStatus(Status::failed);
}

CommandProgressRow::Status CommandProgressRow::getStatus() const {
    return status;
}

void CommandProgressRow::showResponse(const juce::String &response, const juce::Colour &colour) {
    progressBar.setVisible(false);
    percentageLabel.setVisible(false);

    responseLabel.setVisible(true);
    responseLabel.setText(response);
    responseLabel.setColour(juce::Label::textColourId, colour);
    responseLabel.repaint();

    resized();
}

void CommandProgressRow::setStatus(Status status_) {
    status = status_;

    switch (status) {
        case Status::inProgress:
            icon.setIcon(BinaryData::progress_svg);
            icon.setTooltip("In Progress");
            break;

        case Status::complete:
            icon.setIcon(BinaryData::tick_green_svg);
            icon.setTooltip("Complete");
            break;

        case Status::failed:
            icon.setIcon(BinaryData::warning_orange_svg);
            icon.setTooltip("Failed");
            break;
    }
}
