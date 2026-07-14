#include "../CustomLookAndFeel.h"
#include "CommandProgressTable.h"
#include "Icon.h"
#include "SimpleLabel.h"
#include "SimpleProgressBar.h"

CommandProgressTable::CommandProgressTable() {
    addAndMakeVisible(table);
    auto colourTagWidth = UILayout::colourTagWidth + 5;
    table.getHeader().addColumn("", static_cast<int>(ColumnId::tag), colourTagWidth, colourTagWidth, colourTagWidth);
    table.getHeader().addColumn("", static_cast<int>(ColumnId::body), 1);
    table.getHeader().addColumn("", static_cast<int>(ColumnId::icon), 50, 50, 50);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setWantsKeyboardFocus(false);
}

void CommandProgressTable::resized() {
    table.setBounds(getLocalBounds());
}

int CommandProgressTable::getIdealHeight() const {
    return (int) rows.size() * table.getRowHeight();
}

void CommandProgressTable::addRow(const juce::Colour &colourTag, const juce::String &connection) {
    Row row;
    row.colourTag = colourTag;
    row.connection = connection;
    rows.push_back(row);
    table.updateContent();
}

void CommandProgressTable::setInProgress(int rowIndex, std::optional<int> percentage) {
    if (auto *row = getRow(rowIndex)) {
        row->status = Status::inProgress;
        row->percentage = percentage;
        table.updateContent();
    }
}

void CommandProgressTable::setComplete(int rowIndex, const juce::String &value) {
    if (auto *row = getRow(rowIndex)) {
        row->status = Status::complete;
        row->response = value;
        table.updateContent();
    }
}

void CommandProgressTable::setFailed(int rowIndex, const juce::String &error) {
    if (auto *row = getRow(rowIndex)) {
        row->status = Status::failed;
        row->response = error;
        table.updateContent();
        table.scrollToEnsureRowIsOnscreen(rowIndex);
    }
}

CommandProgressTable::Status CommandProgressTable::getStatus() const {
    if (std::ranges::any_of(rows, [](Status status) { return status == Status::inProgress; }, &Row::status)) {
        return Status::inProgress;
    }
    if (std::ranges::any_of(rows, [](Status status) { return status == Status::failed; }, &Row::status)) {
        return Status::failed;
    }
    return Status::complete;
}

CommandProgressTable::Status CommandProgressTable::getStatus(const int rowIndex) const {
    if (const auto *row = getRow(rowIndex)) {
        return row->status;
    }
    return Status::failed;
}

CommandProgressTable::Row *CommandProgressTable::getRow(int index) {
    if (index >= 0 && index < (int) rows.size()) {
        return &rows[(size_t) index];
    }
    return nullptr;
}

const CommandProgressTable::Row *CommandProgressTable::getRow(int index) const {
    if (index >= 0 && index < (int) rows.size()) {
        return &rows[(size_t) index];
    }
    return nullptr;
}

int CommandProgressTable::getNumRows() {
    return (int) rows.size();
}

void CommandProgressTable::paintRowBackground(juce::Graphics& g, int rowIndex, int, int height, bool) {
    if (auto *row = getRow(rowIndex)) {
        g.setColour(row->colourTag);
        g.fillRect(0, 0, UILayout::colourTagWidth, height);
    }
}

void CommandProgressTable::paintCell(juce::Graphics &, int, int, int, int, bool) {
}

juce::Component *CommandProgressTable::refreshComponentForCell(int rowIndex, int columnId, bool, juce::Component *existingComponentToUpdate) {
    delete existingComponentToUpdate;

    if (auto *row = getRow(rowIndex)) {
        switch (static_cast<ColumnId>(columnId)) {
            case ColumnId::tag:
                return nullptr;

            case ColumnId::body:
                class Body : public juce::Component {
                public:
                    Body(const Row &row_)
                        : row(row_) {
                        addAndMakeVisible(connectionLabel);

                        switch (row.status) {
                            case Status::inProgress:
                                if (row.percentage) {
                                    addAndMakeVisible(progressBar);
                                    progressBar.setProgress(row.percentage.value() / 100.0f);

                                    addAndMakeVisible(percentageLabel);
                                    percentageLabel.setText(juce::String(*row.percentage) + "%");
                                }
                                break;

                            case Status::complete:
                                if (row.response != "null") {
                                    addAndMakeVisible(responseLabel);
                                    responseLabel.setText(row.response);
                                    responseLabel.setColour(juce::Label::textColourId, UIColours::success);
                                }
                                break;

                            case Status::failed:
                                addAndMakeVisible(responseLabel);
                                responseLabel.setText(row.response);
                                responseLabel.setColour(juce::Label::textColourId, UIColours::warning);
                                break;
                        }
                    }

                    void resized() {
                        auto bounds = getLocalBounds();

                        if (responseLabel.isVisible()) {
                            responseLabel.setBounds(bounds.removeFromRight((int) std::ceil(responseLabel.getTextWidth())));
                        } else {
                            percentageLabel.setBounds(bounds.removeFromRight(40));
                            progressBar.setBounds(bounds.removeFromRight(125).withSizeKeepingCentre(125, 4));
                        }

                        bounds.removeFromRight(10);
                        connectionLabel.setBounds(bounds);
                    }

                private:
                    const Row row;
                    SimpleLabel connectionLabel{row.connection};
                    SimpleProgressBar progressBar;
                    SimpleLabel percentageLabel{"", UIFonts::getDefaultFont(), juce::Justification::centredRight};
                    SimpleLabel responseLabel{"", UIFonts::getDefaultFont(), juce::Justification::centredRight};
                };

                return new Body(*row);

            case ColumnId::icon:
                switch (row->status) {
                    case Status::inProgress:
                        return new Icon(BinaryData::progress_svg, "In Progress", 0.6f);

                    case Status::complete:
                        return new Icon(BinaryData::tick_green_svg, "Complete", 0.6f);

                    case Status::failed:
                        return new Icon(BinaryData::warning_orange_svg, "Failed", 0.6f);
                }
        }
    }

    return nullptr;
}
