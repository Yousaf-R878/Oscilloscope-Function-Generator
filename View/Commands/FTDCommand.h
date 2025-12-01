#pragma once

#include <string>
#include <vector>

// Forward declaration
class FTDController;

// Base command class
class FTDCommand {
public:
    virtual ~FTDCommand() = default;
    virtual void execute(FTDController* controller) = 0;
    virtual std::string getName() const = 0;
};

