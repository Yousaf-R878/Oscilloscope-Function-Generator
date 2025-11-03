#ifndef FTD_LED_CONTROLLER_H
#define FTD_LED_CONTROLLER_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "ftd2xx.h"

class FTDLEDController {
public:
    explicit FTDLEDController(FT_HANDLE handle);
    void interactiveControl();   // interactive mode (your original control loop)
    void setPin(int pin, bool state);
    void reset();
    void displayPinStates() const;

private:
    FT_HANDLE ftHandle;
    uint8_t outputBuffer;
    static constexpr int NUMBER_OF_PINS = 8;
    static constexpr int PIN_ID_FIRST = 0;
    static constexpr int PIN_ID_LAST = 7;

    void writeStateToDevice();
};

#endif
