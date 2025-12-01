#pragma once

#include "FTDCommand.h"

// Samples command
class FTDSamplesCommand : public FTDCommand {
private:
    int numberOfSamples;
    int intervalMicros;
public:
    FTDSamplesCommand(int numberOfSamples, int intervalMicros)
        : numberOfSamples(numberOfSamples), intervalMicros(intervalMicros) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "samples"; }
    int getNumberOfSamples() const { return numberOfSamples; }
    int getIntervalMicros() const { return intervalMicros; }
};

