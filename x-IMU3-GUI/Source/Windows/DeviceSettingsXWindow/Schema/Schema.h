#pragma once

#include "Group.h"

class Schema {
public:
    struct Group {};
    struct Setting {};

    static Group load(const juce::File&)
    {}





};


inline Group loadSchema(const juce::File&) {
    return {}; // TODO
}
