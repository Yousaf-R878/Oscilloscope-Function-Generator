#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <ftd2xx.h>

class FTDReader {
private:
    FT_HANDLE ftHandle;
    std::vector<unsigned char> buffer; // Buffer to store read data
    DWORD bytesRead;                    // Number of bytes actually read

public:
    // Constructor accepts the FT_HANDLE and optional buffer size
    FTDReader(FT_HANDLE handle, size_t bufferSize = 1);

    // Setter for buffer size
    void setBufferSize(size_t bufferSize);

    // Getter for number of bytes read
    DWORD getBytesRead() const;

    // Getter for buffer content
    const std::vector<unsigned char>& getBuffer() const;

    // Perform the read operation
    FT_STATUS read();

    unsigned char readFromFile(const std::string& filename);
};

