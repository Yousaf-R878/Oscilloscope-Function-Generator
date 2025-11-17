#include <FTDController.h>
#include <Data.h>
#include <fstream>
#include "FTDCommand.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

FTDController::FTDController() : ftHandle(nullptr), numberOfSamples(0) {
    initializeDevice();
    writer = std::make_unique<FTDWriter>(ftHandle);
    reader = std::make_unique<FTDReader>(ftHandle);
    ledController = std::make_unique<FTDLEDController>(ftHandle);
    morseCode = std::make_unique<FTDMorseCode>(ftHandle);
    pipe = std::make_unique<Pipe>();
    oscilloscope = std::make_unique<FTDOscilloscope>(reader.get(), writer.get());
}

FTDController::~FTDController() {
    closeDevice();
}

void FTDController::checkStatus(FT_STATUS status, const std::string& message) {
    if (status != FT_OK) {
        throw std::runtime_error(message + " (FT_STATUS=" + std::to_string(status) + ")");
    }
}

void FTDController::initializeDevice() {
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

void FTDController::closeDevice() {
    if (ftHandle) {
        FT_Close(ftHandle);
        std::cout << "Device closed.\n";
        ftHandle = nullptr;
    }
}

void FTDController::controlLED() {
    std::cout << "[Stub] controlLED called.\n";
    ledController->interactiveControl();
}

void FTDController::sendMorseCode() {
    std::cout << "[Stub] sendMorseCode called.\n";
    morseCode->interactiveSend();
}

void FTDController::driverTest() {
    std::cout << "\n--- Running FTDI Test Driver ---\n";

    // Simulate two FTDI devices (for now we use the same handle)
    FTDReader inputReader(ftHandle);
    FTDWriter outputWriter(ftHandle);

    unsigned char byteFromFile = inputReader.readFromFile("input.txt");
    outputWriter.setByte(byteFromFile);
    outputWriter.write();
    outputWriter.writeToFile("output.txt");

    std::cout << "--- Test Completed ---\n";
}

void FTDController::addProcess(std::shared_ptr<Process> p) {
    if (!pipe) pipe = std::make_unique<Pipe>();
    pipe->addProcess(p);
}

void FTDController::runProcess(Process& process){
    reader->read();
    auto buffer = reader->getBuffer(); 
    Data data(buffer[0]);  //ONLY READING 1 BYTE 

    // Step 2: execute the process
    process.execute(data);

    // Step 3: write back clamped byte
    writer->setByte(static_cast<unsigned char>(data.getValue()));
    writer->write();

    std::cout << "Process applied. New byte value: "
              << static_cast<int>(data.getValue()) << "\n";
}

void FTDController::runMenu() {
    int choice = 0;
    while (true) {
        std::cout << "\nControl Menu\n"
                  << "1. Control LEDs\n"
                  << "2. Send Morse Code\n"
                  << "3. Write byte to port\n"
                  << "4. Read byte from port\n"
                  << "5. Driver Test\n"
                  << "6. Add Shift filter\n"
                  << "7. Add Scale filter\n"
                  << "8. Run Pipe\n"
                  << "9. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 9) break;

        switch (choice) {
            case 1:
                controlLED();
                break;
            case 2:
                sendMorseCode();
                break;
            case 3:
                writer->write();
                break;
            case 4:
                reader->read();
                reader->getBuffer();
                break;
            case 5:
                driverTest();
                break;
            case 6: { // Shift
                int offset;
                std::cout << "Enter shift offset: ";
                std::cin >> offset;
                pipe->addProcess(std::make_shared<Shift>(offset));
                std::cout << "Added Shift filter (" << offset << ")\n";
                break;
            }
            case 7: { // Scale
                int factor;
                std::cout << "Enter scale factor: ";
                std::cin >> factor;
                pipe->addProcess(std::make_shared<Scale>(factor));
                std::cout << "Added Scale filter (" << factor << ")\n";
                break;
            }
            case 8: {
                std::cout << "Running pipe...\n";

                // Read one byte from the FTDI device
                reader->read();
                auto dataBuffer = reader->getBuffer();
                if (dataBuffer.empty()) {
                    std::cout << "No data available to process.\n";
                    break;
                }

                Data data(dataBuffer[0]);
                std::cout << "Original data: " << data.getValue() << "\n";

                // Execute all filters
                pipe->execute(data);

                // Clamp and write result
                unsigned char result = data.getValue();
                std::cout << "Processed (clamped) data: " << (int)result << "\n";

                writer->setByte(result);
                writer->write();

                std::cout << "Processed byte written to FTDI device.\n";

                pipe->clear();
                
                std::cout << "Pipe cleared.\n";
                break;
            }
            default:
                std::cout << "Invalid choice. Try again.\n";
                break;
        }
    }
}

// Command execution methods
void FTDController::startOscilloscope() {
    if (oscilloscope) {
        oscilloscope->start();
    }
}

void FTDController::stopOscilloscope() {
    if (oscilloscope) {
        oscilloscope->stop();
    }
}

void FTDController::readBytes(int count) {
    if (!reader) {
        throw std::runtime_error("Reader not initialized");
    }
    
    // Set buffer size and read
    reader->setBufferSize(count);
    reader->read();
    
    auto buffer = reader->getBuffer();
    std::cout << "Read " << buffer.size() << " bytes from FTDI device." << std::endl;
}

void FTDController::writeBytes(int count) {
    if (!writer) {
        throw std::runtime_error("Writer not initialized");
    }
    
    // Write count bytes (using the current byte value)
    for (int i = 0; i < count; i++) {
        writer->write();
    }
    
    std::cout << "Wrote " << count << " bytes to FTDI device." << std::endl;
}

void FTDController::setNumberOfSamples(int numberOfSamples) {
    this->numberOfSamples = numberOfSamples;
    std::cout << "Number of samples set to: " << numberOfSamples << std::endl;
    
    // If oscilloscope is already running, start collecting data
    if (oscilloscope && oscilloscope->isRunning()) {
        oscilloscope->collectData(numberOfSamples);
    }
}

void FTDController::readFromFile(const std::string& filename) {
    if (!reader) {
        throw std::runtime_error("Reader not initialized");
    }
    
    unsigned char byte = reader->readFromFile(filename);
    writer->setByte(byte);
    std::cout << "Read from file and set byte: 0x" << std::hex 
              << static_cast<int>(byte) << std::dec << std::endl;
}

void FTDController::writeToFile(const std::string& filename) {
    if (!writer) {
        throw std::runtime_error("Writer not initialized");
    }
    
    // If oscilloscope has collected data, write it
    if (oscilloscope && !oscilloscope->getCollectedData().empty()) {
        auto data = oscilloscope->getCollectedData();
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        std::cout << "Wrote " << data.size() << " bytes to file: " << filename << std::endl;
    } else {
        // Write current byte to file
        writer->writeToFile(filename);
    }
}

void FTDController::executeCommand(FTDCommand* command) {
    if (command) {
        command->execute(this);
    }
}

void FTDController::executeCommands(const std::vector<std::unique_ptr<FTDCommand>>& commands) {
    for (const auto& command : commands) {
        if (command) {
            executeCommand(command.get());
        }
    }
}
