#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Icon.h"
#include "SimpleLabel.h"
#include "SimpleProgressBar.h"

class CommandProgressRow : public juce::Component {
public:
    enum class Status {
        inProgress,
        complete,
        failed,
    };

    CommandProgressRow(const juce::Colour &colourTag_, const juce::String &connection);

    void paint(juce::Graphics &g) override;

    void resized() override;

    void setInProgress(std::optional<int> percentage);

    void setComplete(const juce::String &value);

    void setFailed(const juce::String &error);

    Status getStatus() const;

private:
    const juce::Colour colourTag;
    Status status = Status::inProgress;

    SimpleLabel connectionLabel;
    SimpleProgressBar progressBar;
    SimpleLabel percentageLabel{"", UIFonts::getDefaultFont(), juce::Justification::centredRight};
    SimpleLabel responseLabel{"", UIFonts::getDefaultFont(), juce::Justification::centredRight};
    Icon icon{BinaryData::progress_svg, "In Progress", 0.6f};

    void showResponse(const juce::String &response, const juce::Colour &colour);

    void setStatus(Status status_);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandProgressRow)
};
