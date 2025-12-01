#pragma once

#include "FTDCommand.h"

// Start command
class FTDStartCommand : public FTDCommand {
public:
    void execute(FTDController* controller) override;
    std::string getName() const override { return "start"; }
};

