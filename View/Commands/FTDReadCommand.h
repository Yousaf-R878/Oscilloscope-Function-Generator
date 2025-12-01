#pragma once

#include "FTDCommand.h"

// Read command
class FTDReadCommand : public FTDCommand {
private:
    int count;
public:
    FTDReadCommand(int count) : count(count) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "read"; }
};

