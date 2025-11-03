#pragma once

#include <Data.h>

class Process {
public:
    virtual void execute(Data& data) = 0;
    virtual ~Process() = default;
        inline unsigned char clampToByte(int value) {
        if (value < 0) return 0;
        if (value > 255) return 255;
        return static_cast<unsigned char>(value);
    }
};
