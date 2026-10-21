#include "CustomLookAndFeel.h"
#include "CommandProgressTable.h"

CommandProgressTable::CommandProgressTable() {
    setViewedComponent(&content, false);
    setWantsKeyboardFocus(false);
}

void CommandProgressTable::paint(juce::Graphics &g) {
    g.fillAll(UIColours::backgroundDark);
}

void CommandProgressTable::resized() {
    Viewport::resized();

    const auto contentHeight = getIdealHeight();
    const auto scrollBarWidth = contentHeight > getHeight() ? getScrollBarThickness() : 0;
    content.setSize(getWidth() - scrollBarWidth, contentHeight);

    for (size_t index = 0; index < rows.size(); index++) {
        rows[index]->setBounds(0, (int) index * rowHeight, content.getWidth(), rowHeight);
    }
}

int CommandProgressTable::getIdealHeight() const {
    return (int) rows.size() * rowHeight;
}

void CommandProgressTable::addRow(const juce::Colour &colourTag, const juce::String &connection) {
    content.addAndMakeVisible(*rows.emplace_back(std::make_unique<CommandProgressRow>(colourTag, connection)));
    resized();
}

void CommandProgressTable::setInProgress(int rowIndex, std::optional<int> percentage) {
    if (auto *row = getRow(rowIndex)) {
        row->setInProgress(percentage);
    }
}

void CommandProgressTable::setComplete(int rowIndex, const juce::String &value) {
    if (auto *row = getRow(rowIndex)) {
        row->setComplete(value);
    }
}

void CommandProgressTable::setFailed(int rowIndex, const juce::String &error) {
    if (auto *row = getRow(rowIndex)) {
        row->setFailed(error);

        if (row->getY() < getViewPositionY()) {
            setViewPosition(0, row->getY());
        } else if (row->getBottom() > getViewPositionY() + getViewHeight()) {
            setViewPosition(0, row->getBottom() - getViewHeight());
        }
    }
}

CommandProgressRow::Status CommandProgressTable::getStatus() const {
    if (std::ranges::any_of(rows, [](CommandProgressRow::Status status) { return status == CommandProgressRow::Status::inProgress; }, &CommandProgressRow::getStatus)) {
        return CommandProgressRow::Status::inProgress;
    }
    if (std::ranges::any_of(rows, [](CommandProgressRow::Status status) { return status == CommandProgressRow::Status::failed; }, &CommandProgressRow::getStatus)) {
        return CommandProgressRow::Status::failed;
    }
    return CommandProgressRow::Status::complete;
}

CommandProgressRow::Status CommandProgressTable::getStatus(const int rowIndex) const {
    if (const auto *row = getRow(rowIndex)) {
        return row->getStatus();
    }
    return {};
}

CommandProgressRow *CommandProgressTable::getRow(int index) const {
    if (index >= 0 && index < (int) rows.size()) {
        return rows[(size_t) index].get();
    }
    return {};
}
