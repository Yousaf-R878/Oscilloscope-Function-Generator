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

// Start command
class StartCommand : public FTDCommand {
public:
    void execute(FTDController* controller) override;
    std::string getName() const override { return "start"; }
};

// Stop command
class StopCommand : public FTDCommand {
public:
    void execute(FTDController* controller) override;
    std::string getName() const override { return "stop"; }
};

// Read command
class ReadCommand : public FTDCommand {
private:
    int count;
public:
    ReadCommand(int count) : count(count) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "read"; }
};

// Write command
class WriteCommand : public FTDCommand {
private:
    int count;
public:
    WriteCommand(int count) : count(count) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "write"; }
};

// Samples command
class SamplesCommand : public FTDCommand {
private:
    int numberOfSamples;
public:
    SamplesCommand(int numberOfSamples) : numberOfSamples(numberOfSamples) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "samples"; }
    int getNumberOfSamples() const { return numberOfSamples; }
};

// ReadFile command
class ReadFileCommand : public FTDCommand {
private:
    std::string filename;
public:
    ReadFileCommand(const std::string& filename) : filename(filename) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "readFile"; }
};

// WriteFile command
class WriteFileCommand : public FTDCommand {
private:
    std::string filename;
public:
    WriteFileCommand(const std::string& filename) : filename(filename) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "writeFile"; }
};

