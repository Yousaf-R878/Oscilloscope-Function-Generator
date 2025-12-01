#pragma once 

#include "FTDProcess.h"
class Shift : public Process {
    int offset;
public:
    Shift(int o) : offset(o) {}
    void execute(Data& data) override {
        int val = data.getValue();
        val += offset;
        data.setValue(clampToByte(val));
    }
};
