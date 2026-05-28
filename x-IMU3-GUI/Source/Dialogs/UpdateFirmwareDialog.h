#pragma once

#include "Dialog.h"
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/FileSelector.h"
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class UpdateFirmwareDialog : public Dialog {
public:
    UpdateFirmwareDialog();

    void resized() override;

    std::shared_ptr<ximu3::ConnectionConfig> getConnectionConfig() const;

    juce::File getHexFile() const;

    static void launch(juce::ThreadPool &threadPool);

private:
    std::vector<ximu3::Device> devices;
    std::function<void(const std::vector<ximu3::Device> &)> portScannerCallback;
    ximu3::PortScanner portScanner;

    SimpleLabel deviceLabel{"Device:"};
    CustomComboBox deviceValue{"No Devices Found"};

    SimpleLabel hexFileLabel{"Hex File:"};
    FileSelector hexFileSelector{"Select Firmware File", ".hex"};

    Icon warningIcon{BinaryData::warning_white_svg, "", 0.7f};
    SimpleLabel warningLabel{"Updating firmware will restore default settings"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdateFirmwareDialog)
};
