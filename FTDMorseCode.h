#ifndef FTD_MORSE_CODE_H
#define FTD_MORSE_CODE_H

#include <string>
#include "ftd2xx.h"

class FTDMorseCode {
public:
    explicit FTDMorseCode(FT_HANDLE handle);

    void sendCharacter(char c);          // Send a single character in Morse
    void sendMessage(const std::string &message); // Send an entire message
    void interactiveSend();              // User-driven input (like your original loop)

private:
    FT_HANDLE ftHandle;

    static constexpr int DOT_TIME_US = 100000;   // 1 unit for dot
    static constexpr int DASH_TIME_US = 300000;  // 3 units for dash
    static constexpr int INTER_ELEMENT_SPACE_US = 100000; // between dots/dashes
    static constexpr int INTER_LETTER_SPACE_US = 300000;  // between letters

    const char* getMorseCode(char c) const;

    void writeLED(bool on); // Helper to turn LED on/off
    void usleepMicro(int microseconds) const; // cross-platform usleep
};

#endif
