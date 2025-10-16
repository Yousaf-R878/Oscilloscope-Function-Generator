#include "FTDReader.h"

FTDReader::FTDReader(FT_HANDLE handle, size_t bufferSize)
    : ftHandle(handle), buffer(bufferSize, 0), bytesRead(0) {
    if (ftHandle == nullptr) {
        throw std::runtime_error("Invalid FT_HANDLE provided to FTDReader.");
    }
}

void FTDReader::setBufferSize(size_t bufferSize) {
    buffer.resize(bufferSize, 0);
}

DWORD FTDReader::getBytesRead() const {
    return bytesRead;
}

const std::vector<unsigned char>& FTDReader::getBuffer() const {
    return buffer;
}

FT_STATUS FTDReader::read() {
    // Purge RX buffer before reading (optional, depending on use case)
    FT_STATUS status = FT_Purge(ftHandle, FT_PURGE_RX);
    if (status != FT_OK) {
        throw std::runtime_error("FT_Purge failed before read with status: " + std::to_string(status));
    }

    DWORD numberOfBytesRead = 0;
    status = FT_Read(ftHandle, buffer.data(), static_cast<DWORD>(buffer.size()), &numberOfBytesRead);
    if (status != FT_OK) {
        throw std::runtime_error("FT_Read failed with status: " + std::to_string(status));
    }

    bytesRead = numberOfBytesRead;

    std::cout << "Read " << bytesRead << " byte(s): ";
    for (DWORD i = 0; i < bytesRead; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::dec << "\n";

    return status;
}

unsigned char FTDReader::readFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open input file: " + filename);
    }

    unsigned char byte = 0;
    file.read(reinterpret_cast<char*>(&byte), 1);
    if (!file) {
        throw std::runtime_error("Failed to read one byte from " + filename);
    }

    std::cout << "Read byte 0x" << std::hex << static_cast<int>(byte) << std::dec
              << " from " << filename << "\n";
    return byte;
}