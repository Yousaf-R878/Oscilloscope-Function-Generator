#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "ftd2xx.h"

// Helper to print pin states
void displayPinStates(uint8_t outputBuffer, int numberOfPins) {
    for (int pin = 0; pin < numberOfPins; ++pin) {
        std::cout << "Pin " << pin << " = "
                  << ((outputBuffer & (1 << pin)) ? "ON" : "OFF") << "\n";
    }
}

// Main LED control function
void controlLED(FT_HANDLE ftHandle) {
    const int NUMBER_OF_PINS = 8;
    const int PIN_ID_FIRST = 0;
    const int PIN_ID_LAST = 7;
    const int PIN_STATE_ON = 1;
    const int PIN_STATE_OFF = 0;

    std::string input;
    int pin = 0;
    int state = 0;
    FT_STATUS ftStatus;
    DWORD bytesWritten = 0;
    uint8_t outputBuffer = 0x00; // Start with all pins off

    while (true) {
        std::cout << "\nEnter pin number (0-7), 'reset' to turn off all pins, or 'done' to finish:\n> ";
        std::getline(std::cin, input);

        if (input == "done") {
            std::cout << "Exiting LED control...\n";
            break;
        }

        if (input == "reset") {
            outputBuffer = 0x00;
            std::cout << "All pins reset to OFF.\n";

            ftStatus = FT_Write(ftHandle, &outputBuffer, sizeof(outputBuffer), &bytesWritten);
            if (ftStatus != FT_OK) {
                throw std::runtime_error("Error: FT_Write failed during reset");
            }
            continue;
        }

        try {
            pin = std::stoi(input);
        } catch (...) {
            std::cout << "Invalid input. Please enter a number between " << PIN_ID_FIRST << " and " << PIN_ID_LAST << ".\n";
            continue;
        }

        if (pin < PIN_ID_FIRST || pin > PIN_ID_LAST) {
            std::cout << "Error: Invalid pin number " << pin
                      << ". Please enter a value between " << PIN_ID_FIRST << " and " << PIN_ID_LAST << ".\n";
            continue;
        }

        std::cout << "Current pin states:\n";
        displayPinStates(outputBuffer, NUMBER_OF_PINS);

        std::cout << "Enter new state for pin " << pin << " (1 for ON, 0 for OFF):\n> ";
        std::getline(std::cin, input);

        try {
            state = std::stoi(input);
        } catch (...) {
            std::cout << "Invalid input. Please enter 0 or 1.\n";
            continue;
        }

        if (state != PIN_STATE_OFF && state != PIN_STATE_ON) {
            std::cout << "Error: Invalid state " << state
                      << " for pin " << pin << ". Use " << PIN_STATE_OFF << " for OFF or "
                      << PIN_STATE_ON << " for ON.\n";
            continue;
        }

        // Update pin state
        if (state == PIN_STATE_ON) {
            outputBuffer |= (1 << pin);
            std::cout << "Pin " << pin << " is ON.\n";
        } else {
            outputBuffer &= ~(1 << pin);
            std::cout << "Pin " << pin << " is OFF.\n";
        }

        ftStatus = FT_Write(ftHandle, &outputBuffer, sizeof(outputBuffer), &bytesWritten);
        if (ftStatus != FT_OK) {
            throw std::runtime_error("Error: FT_Write failed while updating pin state");
        }
    }

    std::cout << "Final LED states updated. Wrote "
              << bytesWritten << " bytes: 0x"
              << std::hex << static_cast<int>(outputBuffer) << std::dec << "\n";
}
