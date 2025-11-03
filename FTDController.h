#ifndef FTD_DEVICE_MANAGER_H
#define FTD_DEVICE_MANAGER_H

#include <iostream>
#include <stdexcept>
#include <memory>
#include <string>
#include "ftd2xx.h"
#include <FTDWriter.h>
#include <FTDReader.h>
#include "FTDLEDController.h"
#include "FTDMorseCode.h"
#include "FTDProcess.h"
#include "FTDShift.h"
#include "FTDScale.h"

class FTDController {
public:
    FTDController();
    ~FTDController();

    void runMenu();
    void controlLED();
    void sendMorseCode();
    void driverTest();
    void runProcess(Process& process);

private:
    FT_HANDLE ftHandle;
    std::unique_ptr<FTDWriter> writer;
    std::unique_ptr<FTDReader> reader;
    std::unique_ptr<FTDLEDController> ledController;
    std::unique_ptr<FTDMorseCode> morseCode;


    void initializeDevice();
    void closeDevice();

    // Helper for error checking
    void checkStatus(FT_STATUS status, const std::string& message);
};

#endif
