#pragma once

#include "DataLoggerSettings.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/FileSelector.h"
#include "Widgets/Icon.h"
#include "Widgets/SimpleLabel.h"

class DataLoggerSettingsDialog : public Dialog {
public:
    explicit DataLoggerSettingsDialog(const DataLoggerSettings &settings);

    void resized() override;

    DataLoggerSettings getSettings() const;

private:
    SimpleLabel destinationLabel{"Destination:"};
    FileSelector destinationSelector{"Select Destination", {}};
    SimpleLabel nameLabel{"Name:"};
    CustomTextEditor nameValue;
    SimpleLabel timeLabel{"Time:"};
    CustomTextEditor timeValue;
    CustomComboBox timeUnit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataLoggerSettingsDialog)
};
