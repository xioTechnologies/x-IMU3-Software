#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace Firmware
{
    static const juce::String fileName = "x-IMU3-Firmware-v0.7.hex";
    static const juce::String version = "v0.7";
    static const juce::MemoryBlock memoryBlock { BinaryData::xIMU3Firmwarev0_7_hex, BinaryData::xIMU3Firmwarev0_7_hexSize };
}
