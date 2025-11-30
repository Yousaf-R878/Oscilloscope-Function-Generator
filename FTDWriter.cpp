#include <iostream>
#include <string>
#include <stdexcept>
#include <ftd2xx.h>
#include "FTDWriter.h"

// Constructor
FTDWriter::FTDWriter(FT_HANDLE handle, unsigned char byte)
    : ftHandle(handle), byteToWrite(byte), bytesWritten(0) {
    if (ftHandle == nullptr) {
        throw std::runtime_error("Invalid FT_HANDLE provided to FTDWriter.");
    }
}

// Setter
void FTDWriter::setByte(unsigned char byte) {
    byteToWrite = byte;
}

// Getter
DWORD FTDWriter::getBytesWritten() const {
    return bytesWritten;
}

// Write operation
FT_STATUS FTDWriter::write() {
    FT_STATUS ftStatus = FT_Write(ftHandle, &byteToWrite, sizeof(byteToWrite), &bytesWritten);
    if (ftStatus != FT_OK) {
        throw std::runtime_error("FT_Write failed with status: " + std::to_string(ftStatus));
    }
    std::cout << "Wrote " << bytesWritten << " byte(s): 0x" 
              << std::hex << static_cast<int>(byteToWrite) << std::dec << "\n";
    return ftStatus;
}

void FTDWriter::writeToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open output file: " + filename);
    }

    file.write(reinterpret_cast<const char*>(&byteToWrite), 1);
    if (!file) {
        throw std::runtime_error("Failed to write to " + filename);
    }

    std::cout << "Byte 0x" << std::hex << static_cast<int>(byteToWrite)
              << std::dec << " written to " << filename << "\n";
}
