#pragma once

#include "FTDCommand.h"

// Scope command with wait functionality
class FTDScopeCommand : public FTDCommand {
private:
    int sampleIntervalMicros;
    int waitTimeMicros;
public:
    FTDScopeCommand(int sampleIntervalMicros, int waitTimeMicros)
        : sampleIntervalMicros(sampleIntervalMicros), waitTimeMicros(waitTimeMicros) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "scope"; }
    int getSampleIntervalMicros() const { return sampleIntervalMicros; }
    int getWaitTimeMicros() const { return waitTimeMicros; }
};

