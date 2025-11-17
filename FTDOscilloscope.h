#pragma once

#include <vector>
#include <string>
#include "ftd2xx.h"

class FTDReader;
class FTDWriter;

class FTDOscilloscope {
public:
    FTDOscilloscope(FTDReader* reader, FTDWriter* writer);
    
    // Start data collection
    void start();
    
    // Stop data collection
    void stop();
    
    // Collect data for specified number of samples
    void collectData(int numberOfSamples);
    
    // Check if oscilloscope is running
    bool isRunning() const { return running; }
    
    // Get collected data
    const std::vector<unsigned char>& getCollectedData() const { return collectedData; }
    
    // Clear collected data
    void clearData() { collectedData.clear(); }
    
private:
    FTDReader* reader;
    FTDWriter* writer;
    bool running;
    std::vector<unsigned char> collectedData;
};

