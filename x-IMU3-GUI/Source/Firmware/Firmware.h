#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace Firmware
{
    static const juce::String fileName = "x-IMU3-Firmware-v2.1.1.hex";
    static const juce::String version = "v2.1.1";
    static const juce::MemoryBlock memoryBlock { BinaryData::xIMU3Firmwarev2_1_1_hex, BinaryData::xIMU3Firmwarev2_1_1_hexSize };
}
