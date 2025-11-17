#ifndef FTD_DEVICE_MANAGER_H
#define FTD_DEVICE_MANAGER_H

#include <iostream>
#include <stdexcept>
#include <memory>
#include <string>
#include "ftd2xx.h"
#include <FTDWriter.h>
#include <FTDReader.h>
#include "FTDProcess.h"
#include "FTDShift.h"
#include "FTDScale.h"
#include <FTDPipe.h> 
#include <Data.h>   
#include <vector>
#include "FTDOscilloscope.h"

// Forward declaration
class FTDCommand;

class FTDController {
public:
    FTDController();
    ~FTDController();

    void runMenu();
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
    
    // Execute commands from parser
    void executeCommand(FTDCommand* command);
    void executeCommands(const std::vector<std::unique_ptr<FTDCommand>>& commands);

private:
    FT_HANDLE ftHandle;
    std::unique_ptr<FTDWriter> writer;
    std::unique_ptr<FTDReader> reader;
    std::unique_ptr<Pipe> pipe;
    std::unique_ptr<FTDOscilloscope> oscilloscope;
    int numberOfSamples;
    int sampleIntervalMicros;

    void initializeDevice();
    void closeDevice();

    // Helper for error checking
    void checkStatus(FT_STATUS status, const std::string& message);
};

#endif
