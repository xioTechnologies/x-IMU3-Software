#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace Firmware {
    static const juce::String hexFile = "x-IMU3-Firmware-v2.5.4.hex";
    static const juce::String version = "v2.5.4";
    static const juce::MemoryBlock memoryBlock{BinaryData::xIMU3Firmwarev2_5_4_hex, BinaryData::xIMU3Firmwarev2_5_4_hexSize};
}
