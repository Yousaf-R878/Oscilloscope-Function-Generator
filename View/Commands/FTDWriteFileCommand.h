#pragma once

#include "FTDCommand.h"
#include <string>

// WriteFile command
class FTDWriteFileCommand : public FTDCommand {
private:
    std::string filename;
public:
    FTDWriteFileCommand(const std::string& filename) : filename(filename) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "writeFile"; }
};

