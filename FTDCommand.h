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
    unsigned char byteValue;
    int count;
public:
    WriteCommand(unsigned char byteValue, int count = 1) : byteValue(byteValue), count(count) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "write"; }
};

// Samples command
class SamplesCommand : public FTDCommand {
private:
    int numberOfSamples;
    int intervalMicros;
public:
    SamplesCommand(int numberOfSamples, int intervalMicros)
        : numberOfSamples(numberOfSamples), intervalMicros(intervalMicros) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "samples"; }
    int getNumberOfSamples() const { return numberOfSamples; }
    int getIntervalMicros() const { return intervalMicros; }
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

// Scope command with wait functionality
class ScopeCommand : public FTDCommand {
private:
    int sampleIntervalMicros;
    int waitTimeMicros;
public:
    ScopeCommand(int sampleIntervalMicros, int waitTimeMicros)
        : sampleIntervalMicros(sampleIntervalMicros), waitTimeMicros(waitTimeMicros) {}
    void execute(FTDController* controller) override;
    std::string getName() const override { return "scope"; }
    int getSampleIntervalMicros() const { return sampleIntervalMicros; }
    int getWaitTimeMicros() const { return waitTimeMicros; }
};

