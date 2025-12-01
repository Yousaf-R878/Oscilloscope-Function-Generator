#pragma once

#include "FTDCommand.h"
#include <string>

// ReadFile command
class FTDReadFileCommand : public FTDCommand {
private:
    std::string filename;
public:
    FTDReadFileCommand(const std::string& filename) : filename(filename) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "readFile"; }
};

