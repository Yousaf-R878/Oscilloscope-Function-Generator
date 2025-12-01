#ifndef FTD_DEVICE_MANAGER_H
#define FTD_DEVICE_MANAGER_H

#include <iostream>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include "../ftd2xx.h"
#include "../Model/FTDWriter.h"
#include "../Model/FTDReader.h"
#include "../Model/FTDProcess.h"
#include "../Model/FTDShift.h"
#include "../Model/FTDScale.h"
#include "../Model/FTDPipe.h"
#include "../Model/Data.h"
#include "../Model/FTDOscilloscope.h"
#include "../Model/FTDOscilloscopeThreaded.h"

// Forward declaration
class FTDCommand;

class FTDController {
public:
    FTDController();
    ~FTDController();

    void controlLED();
    void sendMorseCode();
    void driverTest();
    void runProcess(Process& process);

    void addProcess(std::shared_ptr<Process> p);
    void executePipe();

    // optional accessors if view wants to directly call reader/writer
    FTDReader* getReader() const;
    FTDWriter* getWriter() const;

    // Command execution methods for CLI
    void startOscilloscope();
    void stopOscilloscope();
    void readBytes(int count);
    void writeBytes(unsigned char byteValue, int count = 1);
    void configureSampling(int numberOfSamples, int intervalMicros);
    void readFromFile(const std::string& filename);
    void writeToFile(const std::string& filename);
    
    // Multi-threaded scope with wait
    void runScopeWithWait(int sampleIntervalMicros, int waitTimeMicros);
    
    // Execute commands from parser
    void executeCommand(FTDCommand* command);
    void executeCommands(const std::vector<std::unique_ptr<FTDCommand>>& commands);

private:
    FT_HANDLE ftHandle;
    std::unique_ptr<FTDWriter> writer;
    std::unique_ptr<FTDReader> reader;
    std::unique_ptr<Pipe> pipe;
    std::unique_ptr<FTDOscilloscope> oscilloscope;
    std::unique_ptr<FTDOscilloscopeThreaded> oscilloscopeThreaded;
    int numberOfSamples;
    int sampleIntervalMicros;

    void initializeDevice();
    void closeDevice();

    // Helper for error checking
    void checkStatus(FT_STATUS status, const std::string& message);
};

#endif
