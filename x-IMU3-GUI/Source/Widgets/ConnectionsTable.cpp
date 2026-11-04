#include "ConnectionsTable.h"

ConnectionsTable::ConnectionsTable() {
    addAndMakeVisible(selectAllButton);
    addAndMakeVisible(selectAllLabel);
    addAndMakeVisible(viewport);
    addAndMakeVisible(noConnectionsFoundLabel);

    selectAllButton.setToggleState(true, juce::dontSendNotification);
    selectAllButton.onClick = [&] {
        for (auto &row: rows) {
            if (row->getData().unavailable == false) {
                auto data = row->getData();
                data.selected = selectAllButton.getToggleState();
                row->setData(data);
            }
        }
    };

    viewport.setViewedComponent(&content, false);
    viewport.setWantsKeyboardFocus(false);
}

void ConnectionsTable::paint(juce::Graphics &g) {
    g.setColour(UIColours::backgroundDark);
    g.fillRect(viewport.getBounds());
}

void ConnectionsTable::resized() {
    static constexpr int headerHeight = 30;

    auto bounds = getLocalBounds();
    selectAllButton.setBounds(bounds.removeFromTop(headerHeight));
    selectAllLabel.setBounds(selectAllButton.getBounds().withTrimmedLeft(ConnectionsRow::toggleWidth));
    viewport.setBounds(bounds);
    noConnectionsFoundLabel.setBounds(bounds);

    const auto contentHeight = (int) rows.size() * rowHeight;
    const auto scrollBarWidth = contentHeight > viewport.getHeight() ? viewport.getScrollBarThickness() : 0;
    content.setSize(viewport.getWidth() - scrollBarWidth, contentHeight);

    for (size_t index = 0; index < rows.size(); index++) {
        rows[index]->setBounds(0, (int) index * rowHeight, content.getWidth(), rowHeight);
    }
}

void ConnectionsTable::setRows(std::vector<ConnectionsRow::Data> rows_) {
    std::erase_if(rows, [&](const std::unique_ptr<ConnectionsRow> &row) {
        return std::ranges::find(rows_, row->getData()) == rows_.end();
    });

    for (auto &row: rows) {
        auto data = *std::ranges::find(rows_, row->getData());
        data.selected = row->getData().selected;
        row->setData(data);
    }

    for (auto &data: rows_) {
        if (std::ranges::any_of(rows, [&](const std::unique_ptr<ConnectionsRow> &row) { return row->getData() == data; })) {
            continue;
        }

        data.selected = data.selected || selectAllButton.getToggleState();

        auto &row = *rows.emplace_back(std::make_unique<ConnectionsRow>([this](bool selected) {
            selectAllButton.setToggleState(selectAllButton.getToggleState() && selected, juce::dontSendNotification);
        }));
        row.setData(data);
        content.addAndMakeVisible(row);
    }

    std::stable_partition(rows.rbegin(), rows.rend(), [](const std::unique_ptr<ConnectionsRow> &row) {
        return row->getData().unavailable;
    });

    std::vector<std::unique_ptr<ConnectionsRow> > sorted;

    const auto getParent = [](const juce::String &config) -> juce::String {
        if (config.startsWith("Mux")) {
            return config.fromFirstOccurrenceOf(", ", false, false);
        }
        return {};
    };

    const auto getChildOf = [&](const juce::String &parent, juce::String config) -> juce::String {
        for (; config.isNotEmpty(); config = getParent(config)) {
            if (getParent(config) == parent) {
                return config;
            }
        }
        return {};
    };

    const std::function<void(const juce::String &)> addChildren = [&](const juce::String &parent) {
        for (auto &row: rows) {
            if (row == nullptr) {
                continue;
            }

            const auto child = getChildOf(parent, row->getData().config->toString());
            const auto match = std::ranges::find_if(rows, [&](const std::unique_ptr<ConnectionsRow> &other) {
                return other != nullptr && other->getData().config->toString() == child;
            });

            if (match != rows.end()) {
                sorted.push_back(std::move(*match));
                addChildren(child);
            }
        }
    };

    addChildren({});
    rows = std::move(sorted);

    resized();

    noConnectionsFoundLabel.setVisible(rows.empty());
}

std::vector<ximu3::ConnectionConfig *> ConnectionsTable::getConnectionConfigs() const {
    std::vector<ximu3::ConnectionConfig *> configs;

    for (const auto &row: rows) {
        if (row->getData().selected && row->getData().unavailable == false) {
            configs.push_back(row->getData().config.get());
        }
    }

    return configs;
}
