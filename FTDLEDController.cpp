#include "FTDLEDController.h"

FTDLEDController::FTDLEDController(FT_HANDLE handle)
    : ftHandle(handle), outputBuffer(0x00) {}

void FTDLEDController::writeStateToDevice() {
    FT_STATUS ftStatus;
    DWORD bytesWritten = 0;

    ftStatus = FT_Write(ftHandle, &outputBuffer, sizeof(outputBuffer), &bytesWritten);
    if (ftStatus != FT_OK) {
        throw std::runtime_error("Error: FT_Write failed (FTDLEDController)");
    }
}

void FTDLEDController::displayPinStates() const {
    for (int pin = 0; pin < NUMBER_OF_PINS; ++pin) {
        std::cout << "Pin " << pin << " = "
                  << ((outputBuffer & (1 << pin)) ? "ON" : "OFF") << "\n";
    }
}

void FTDLEDController::setPin(int pin, bool state) {
    if (pin < PIN_ID_FIRST || pin > PIN_ID_LAST) {
        throw std::out_of_range("Invalid pin number. Must be 0–7.");
    }

    if (state) {
        outputBuffer |= (1 << pin);
    } else {
        outputBuffer &= ~(1 << pin);
    }

    writeStateToDevice();

    std::cout << "Pin " << pin << " set to "
              << (state ? "ON" : "OFF") << ".\n";
}

void FTDLEDController::reset() {
    outputBuffer = 0x00;
    writeStateToDevice();
    std::cout << "All pins reset to OFF.\n";
}

void FTDLEDController::interactiveControl() {
    std::string input;
    while (true) {
        std::cout << "\nEnter pin number (0-7), 'reset' to turn off all pins, or 'done' to finish:\n> ";
        std::getline(std::cin, input);

        if (input == "done") {
            std::cout << "Exiting LED control...\n";
            break;
        }

        if (input == "reset") {
            reset();
            continue;
        }

        int pin;
        try {
            pin = std::stoi(input);
        } catch (...) {
            std::cout << "Invalid input. Please enter a number between 0 and 7.\n";
            continue;
        }

        if (pin < PIN_ID_FIRST || pin > PIN_ID_LAST) {
            std::cout << "Error: Invalid pin number " << pin
                      << ". Please enter a value between 0 and 7.\n";
            continue;
        }

        displayPinStates();

        std::cout << "Enter new state for pin " << pin << " (1 for ON, 0 for OFF):\n> ";
        std::getline(std::cin, input);

        int state;
        try {
            state = std::stoi(input);
        } catch (...) {
            std::cout << "Invalid input. Please enter 0 or 1.\n";
            continue;
        }

        if (state != 0 && state != 1) {
            std::cout << "Error: Invalid state " << state << ". Use 0 or 1.\n";
            continue;
        }

        setPin(pin, state);
    }

    std::cout << "Final LED states updated: 0x"
              << std::hex << static_cast<int>(outputBuffer) << std::dec << "\n";
}
