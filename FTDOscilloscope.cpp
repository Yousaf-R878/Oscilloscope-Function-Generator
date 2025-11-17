#include "FTDOscilloscope.h"
#include "FTDReader.h"
#include "FTDWriter.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

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

void FTDOscilloscope::collectData(int numberOfSamples, int intervalMicros) {
    if (!running) {
        std::cout << "Warning: Oscilloscope is not running. Starting it now." << std::endl;
        start();
    }
    
    if (intervalMicros <= 0) {
        throw std::invalid_argument("Interval must be greater than zero microseconds");
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

        if (running && i < numberOfSamples - 1) {
            std::this_thread::sleep_for(std::chrono::microseconds(intervalMicros));
        }
    }
    
    std::cout << "Data collection completed. Collected " << collectedData.size() << " bytes." << std::endl;
}

