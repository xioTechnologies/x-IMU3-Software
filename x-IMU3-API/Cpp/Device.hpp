#pragma once

#include "../C/Ximu3.h"
#include <string>

namespace ximu3 {
    class Device : public XIMU3_Device {
    public:
        explicit Device(XIMU3_Device &device) : XIMU3_Device(XIMU3_device_clone(device)) {
        }

        Device(const Device &other) : XIMU3_Device(XIMU3_device_clone(other)) {
        }

        Device &operator=(const Device &other) {
            if (this != &other) {
                XIMU3_device_free(*this);
                *static_cast<XIMU3_Device *>(this) = XIMU3_device_clone(other);
            }

            return *this;
        }

        ~Device() {
            XIMU3_device_free(*this);
        }

        std::string toString() const {
            return XIMU3_device_to_string(*this);
        }
    };
}
