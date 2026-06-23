#pragma once

class Value {
public:
    virtual ~Value() = default;

    virtual void refresh() = 0;
};
