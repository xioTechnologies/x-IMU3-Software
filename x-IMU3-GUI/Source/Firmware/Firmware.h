#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace Firmware
{
    static const juce::String fileName = "x-IMU3-Firmware-v1.1.3.hex";
    static const juce::String version = "v1.1.3";
    static const juce::MemoryBlock memoryBlock { BinaryData::xIMU3Firmwarev1_1_3_hex, BinaryData::xIMU3Firmwarev1_1_3_hexSize };
}
