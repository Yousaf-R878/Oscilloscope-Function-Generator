#include <iostream>
#include <stdexcept>
#include <memory>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "ftd2xx.h"
#include "FTDWriter.h"
#include "FTDReader.h"

// Function declarations
void initializeDevice(FT_HANDLE& ftHandle);
void controlLED(FT_HANDLE ftHandle);
void sendMorseCode(FT_HANDLE ftHandle);
FT_STATUS Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten);

// Helper for error checking
void checkStatus(FT_STATUS status, const std::string& message) {
    if (status != FT_OK) {
        throw std::runtime_error(message + " (FT_STATUS=" + std::to_string(status) + ")");
    }
}

int main() {
    try {
        FT_HANDLE ftHandle = nullptr;
        initializeDevice(ftHandle);

        FTDWriter writer(ftHandle);
        writer.setByte(0xFF);

        FTDReader reader(ftHandle, 2);

        int choice = 0;
        FT_STATUS ftStatus;
        // unsigned char byteToWrite = 0x00;
        // DWORD bytesWritten = 0;

        // unsigned char byteRead = 0;
        // DWORD numberOfBytesRead = 0;

        while (true) {
            std::cout << "\nControl Menu\n"
                      << "1. Control LEDs\n"
                      << "2. Send Morse Code\n"
                      << "3. Write byte to port\n"
                      << "4. Read byte from port\n"
                      << "5. Exit\n"
                      << "Enter your choice: ";
            std::cin >> choice;

            if (choice == 5)
                break;

            switch (choice) {
                case 1:
                    controlLED(ftHandle);
                    break;
                case 2:
                    sendMorseCode(ftHandle);
                    break;
                case 3:
                    ftStatus = writer.write();
                    break;
                case 4: {
                    reader.read(); // Perform the read
                    auto data = reader.getBuffer(); // Access buffer              
                    break;
                }
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }

        FT_Close(ftHandle);
        std::cout << "Device closed.\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void initializeDevice(FT_HANDLE& ftHandle) {
    FT_STATUS ftStatus;

    ftStatus = FT_Open(0, &ftHandle);
    checkStatus(ftStatus, "FT_Open failed");
    std::cout << "Device opened successfully.\n";

    ftStatus = FT_ResetDevice(ftHandle);
    checkStatus(ftStatus, "FT_ResetDevice failed");
    std::cout << "Device reset successfully.\n";

    ftStatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    checkStatus(ftStatus, "FT_Purge failed");
    std::cout << "Purged USB buffers successfully.\n";

    ftStatus = FT_SetUSBParameters(ftHandle, 64, 0);
    checkStatus(ftStatus, "FT_SetUSBParameters failed");
    std::cout << "USB parameters set successfully.\n";

    ftStatus = FT_SetBitMode(ftHandle, 0xFF, 0x01);
    checkStatus(ftStatus, "FT_SetBitMode failed");
    std::cout << "Set synchronous bit bang mode successfully.\n";
}

// // Dummy stubs (you can implement these as before)
// void controlLED(FT_HANDLE ftHandle) {
//     std::cout << "[Stub] controlLED called.\n";
// }

// void sendMorseCode(FT_HANDLE ftHandle) {
//     std::cout << "[Stub] sendMorseCode called.\n";
// }

// FT_STATUS Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten) {
//     return FT_Write(ftHandle, lpBuffer, dwBytesToWrite, lpdwBytesWritten);
// }
