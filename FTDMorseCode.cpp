#include "FTDMorseCode.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Morse code table for A-Z, 0-9
static const char* morse_code[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",
    "-----", ".----", "..---", "...--", "....-", ".....",
    "-....", "--...", "---..", "----."
};

FTDMorseCode::FTDMorseCode(FT_HANDLE handle) : ftHandle(handle) {}

void FTDMorseCode::usleepMicro(int microseconds) const {
#ifdef _WIN32
    Sleep(microseconds / 1000);
#else
    usleep(microseconds);
#endif
}

void FTDMorseCode::writeLED(bool on) {
    BYTE output = on ? 0x01 : 0x00; // LED on pin 0
    DWORD bytesWritten = 0;
    FT_STATUS ftStatus = FT_Write(ftHandle, &output, sizeof(output), &bytesWritten);
    if (ftStatus != FT_OK) {
        throw std::runtime_error("FT_Write failed in FTDMorseCode");
    }
}

const char* FTDMorseCode::getMorseCode(char c) const {
    if (c >= 'A' && c <= 'Z') return morse_code[c - 'A'];
    if (c >= 'a' && c <= 'z') return morse_code[c - 'a'];
    if (c >= '0' && c <= '9') return morse_code[c - '0' + 26];
    return nullptr;
}

void FTDMorseCode::sendCharacter(char c) {
    const char* morse = getMorseCode(c);
    if (!morse) return; // unsupported character

    std::cout << "Sending '" << c << "': " << morse << "\n";

    while (*morse) {
        if (*morse == '.') {
            writeLED(true);
            usleepMicro(DOT_TIME_US);
        } else if (*morse == '-') {
            writeLED(true);
            usleepMicro(DASH_TIME_US);
        }

        writeLED(false);
        usleepMicro(INTER_ELEMENT_SPACE_US);
        ++morse;
    }

    usleepMicro(INTER_LETTER_SPACE_US);
}

void FTDMorseCode::sendMessage(const std::string &message) {
    for (char c : message) {
        sendCharacter(c);
    }
}

void FTDMorseCode::interactiveSend() {
    std::string input;
    std::cout << "Enter message to send in Morse (type 'E0' to exit):\n";

    while (true) {
        std::getline(std::cin, input);
        if (input == "E0") {
            std::cout << "Exiting Morse code sender...\n";
            break;
        }

        sendMessage(input);
    }
}
