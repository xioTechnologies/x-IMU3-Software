#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace Firmware
{
    static const juce::String hexFile = "x-IMU3-Firmware-v2.5.1.hex";
    static const juce::String version = "v2.5.1";
    static const juce::MemoryBlock memoryBlock { BinaryData::xIMU3Firmwarev2_5_1_hex, BinaryData::xIMU3Firmwarev2_5_1_hexSize };
}
