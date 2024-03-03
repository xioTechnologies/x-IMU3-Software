#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace Firmware
{
    static const juce::String hexFile = "x-IMU3-Firmware-v2.2.0.hex";
    static const juce::String version = "v2.2.0";
    static const juce::MemoryBlock memoryBlock { BinaryData::xIMU3Firmwarev2_2_0_hex, BinaryData::xIMU3Firmwarev2_2_0_hexSize };
}
