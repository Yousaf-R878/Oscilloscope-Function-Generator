#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "../ftd2xx.h"

class FTDWriter {
private:
    FT_HANDLE ftHandle;
    unsigned char byteToWrite;
    DWORD bytesWritten;

public:
    // Constructor accepts a handle and optional initial byte
    FTDWriter(FT_HANDLE handle, unsigned char byte = 0xFF);

    // Setter for the byte to write
    void setByte(unsigned char byte);

    // Getter for the last written byte count
    DWORD getBytesWritten() const;

    // Perform the write operation
    FT_STATUS write();

    void writeToFile(const std::string& filename);
};
