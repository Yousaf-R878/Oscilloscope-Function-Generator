#pragma once

#include "FTDCommand.h"

// Stop command
class FTDStopCommand : public FTDCommand {
public:
    void execute(FTDController* controller) override;
    std::string getName() const override { return "stop"; }
};

