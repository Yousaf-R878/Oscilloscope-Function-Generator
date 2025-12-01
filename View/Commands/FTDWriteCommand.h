#pragma once

#include "FTDCommand.h"

// Write command
class FTDWriteCommand : public FTDCommand {
private:
    unsigned char byteValue;
    int count;
public:
    FTDWriteCommand(unsigned char byteValue, int count = 1) : byteValue(byteValue), count(count) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "write"; }
};

