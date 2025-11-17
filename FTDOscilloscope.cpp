#include "FTDOscilloscope.h"
#include "FTDReader.h"
#include "FTDWriter.h"
#include <iostream>

FTDOscilloscope::FTDOscilloscope(FTDReader* reader, FTDWriter* writer)
    : reader(reader), writer(writer), running(false) {
    if (!reader || !writer) {
        throw std::runtime_error("FTDOscilloscope: reader and writer must not be null");
    }
}

void FTDOscilloscope::start() {
    running = true;
    collectedData.clear();
    std::cout << "Oscilloscope started." << std::endl;
}

void FTDOscilloscope::stop() {
    running = false;
    std::cout << "Oscilloscope stopped." << std::endl;
    std::cout << "Total samples collected: " << collectedData.size() << std::endl;
}

void FTDOscilloscope::collectData(int numberOfSamples) {
    if (!running) {
        std::cout << "Warning: Oscilloscope is not running. Starting it now." << std::endl;
        start();
    }
    
    std::cout << "Collecting " << numberOfSamples << " samples..." << std::endl;
    
    for (int i = 0; i < numberOfSamples && running; i++) {
        try {
            // Read data from FTDI device
            reader->read();
            auto buffer = reader->getBuffer();
            
            // Store collected data
            for (unsigned char byte : buffer) {
                collectedData.push_back(byte);
            }
            
            // Optional: Write data back (can be configured)
            // writer->setByte(buffer[0]);
            // writer->write();
            
        } catch (const std::exception& e) {
            std::cerr << "Error during data collection at sample " << i << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "Data collection completed. Collected " << collectedData.size() << " bytes." << std::endl;
}

