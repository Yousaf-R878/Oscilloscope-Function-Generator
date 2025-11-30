#include "FTDCommandParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <limits>

std::unique_ptr<FTDCommand> FTDCommandParser::parseCommand(const std::string& line) {
    auto tokens = split(line);
    if (tokens.empty()) {
        return nullptr;
    }
    
    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "start") {
        return std::make_unique<StartCommand>();
    }
    else if (command == "stop") {
        return std::make_unique<StopCommand>();
    }
    else if (command == "read") {
        if (tokens.size() < 2) {
            throw std::runtime_error("Read command requires a count argument");
        }
        int count = std::stoi(tokens[1]);
        return std::make_unique<ReadCommand>(count);
    }
    else if (command == "write") {
        if (tokens.size() < 2) {
            throw std::runtime_error("Write command requires a byte value argument");
        }
        unsigned char byteValue = parseByteValue(tokens[1]);
        int count = 1;
        if (tokens.size() >= 3) {
            count = std::stoi(tokens[2]);
        }
        return std::make_unique<WriteCommand>(byteValue, count);
    }
    else if (command == "samples") {
        if (tokens.size() < 3) {
            throw std::runtime_error("Samples command requires count and interval arguments (e.g., samples 1000 5ms)");
        }
        int numberOfSamples = std::stoi(tokens[1]);
        int intervalMicros = parseDurationMicros(tokens[2]);
        return std::make_unique<SamplesCommand>(numberOfSamples, intervalMicros);
    }
    else if (command == "readfile") {
        if (tokens.size() < 2) {
            throw std::runtime_error("ReadFile command requires a filename argument");
        }
        return std::make_unique<ReadFileCommand>(tokens[1]);
    }
    else if (command == "writefile") {
        if (tokens.size() < 2) {
            throw std::runtime_error("WriteFile command requires a filename argument");
        }
        return std::make_unique<WriteFileCommand>(tokens[1]);
    }
    else if (command == "scope") {
        return parseScopeCommand(tokens);
    }
    else {
        throw std::runtime_error("Unknown command: " + command);
    }
}

std::vector<std::unique_ptr<FTDCommand>> FTDCommandParser::parseFile(const std::string& filename) {
    std::vector<std::unique_ptr<FTDCommand>> commands;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        try {
            auto command = parseCommand(line);
            if (command) {
                commands.push_back(std::move(command));
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing line " << lineNumber << ": " << e.what() << std::endl;
            throw;
        }
    }
    
    return commands;
}

std::vector<std::unique_ptr<FTDCommand>> FTDCommandParser::parseStdin() {
    std::vector<std::unique_ptr<FTDCommand>> commands;
    std::string line;
    
    std::cout << "Enter commands (one per line, empty line to finish):" << std::endl;
    while (std::getline(std::cin, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Empty line means done
        if (line.empty()) {
            break;
        }
        
        // Skip comments
        if (line[0] == '#') {
            continue;
        }
        
        try {
            auto command = parseCommand(line);
            if (command) {
                commands.push_back(std::move(command));
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing command: " << e.what() << std::endl;
        }
    }
    
    return commands;
}

std::vector<std::string> FTDCommandParser::split(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    // For scope commands with commas, handle them specially
    if (str.find("scope") != std::string::npos && str.find(',') != std::string::npos) {
        // Scope command with commas - split on commas and spaces
        std::string currentToken;
        for (char c : str) {
            if (c == ',' || c == ' ' || c == '\t') {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
            } else {
                currentToken += c;
            }
        }
        if (!currentToken.empty()) {
            tokens.push_back(currentToken);
        }
    } else {
        // Regular space-separated commands
        while (iss >> token) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

unsigned char FTDCommandParser::parseByteValue(const std::string& str) {
    // Check if it's a hex value (0xFF, 0xff, FF, ff)
    if (str.length() >= 2 && str.substr(0, 2) == "0x") {
        // Hex format: 0xFF
        return static_cast<unsigned char>(std::stoul(str, nullptr, 16));
    }
    else if (str.length() == 2 && 
             ((str[0] >= '0' && str[0] <= '9') || (str[0] >= 'A' && str[0] <= 'F') || (str[0] >= 'a' && str[0] <= 'f')) &&
             ((str[1] >= '0' && str[1] <= '9') || (str[1] >= 'A' && str[1] <= 'F') || (str[1] >= 'a' && str[1] <= 'f'))) {
        // Hex format: FF (two hex digits)
        return static_cast<unsigned char>(std::stoul(str, nullptr, 16));
    }
    else {
        // Decimal format
        int value = std::stoi(str);
        if (value < 0 || value > 255) {
            throw std::runtime_error("Byte value must be between 0 and 255");
        }
        return static_cast<unsigned char>(value);
    }
}

int FTDCommandParser::parseDurationMicros(const std::string& str) {
    if (str.empty()) {
        throw std::runtime_error("Duration value cannot be empty");
    }

    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    long long multiplier = 1; // default microseconds
    std::string numericPortion = lower;

    if (lower.size() >= 2 && lower.substr(lower.size() - 2) == "ms") {
        multiplier = 1000;
        numericPortion = lower.substr(0, lower.size() - 2);
    } else if (lower.size() >= 2 && lower.substr(lower.size() - 2) == "us") {
        multiplier = 1;
        numericPortion = lower.substr(0, lower.size() - 2);
    } else if (lower.back() == 's') {
        multiplier = 1'000'000;
        numericPortion = lower.substr(0, lower.size() - 1);
    }

    if (numericPortion.empty()) {
        throw std::runtime_error("Invalid duration value: " + str);
    }

    double value = std::stod(numericPortion);
    if (value <= 0) {
        throw std::runtime_error("Duration must be greater than zero");
    }

    long long micros = static_cast<long long>(value * multiplier);
    if (micros <= 0 || micros > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Duration out of range for microseconds: " + str);
    }

    return static_cast<int>(micros);
}

std::unique_ptr<FTDCommand> FTDCommandParser::parseScopeCommand(const std::vector<std::string>& tokens) {
    int sampleIntervalMicros = 1000;
    int waitTimeMicros = 5000000;
    
    for (size_t i = 0; i < tokens.size(); i++) {
        std::string token = tokens[i];
        std::string lowerToken = token;
        std::transform(lowerToken.begin(), lowerToken.end(), lowerToken.begin(), ::tolower);
        
        if (lowerToken == "start" || lowerToken == "stop") {
            continue;
        }
        
        size_t eqPos = token.find('=');
        if (eqPos != std::string::npos) {
            std::string key = token.substr(0, eqPos);
            std::string value = token.substr(eqPos + 1);
            
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "sampletime" || key == "sampleinterval") {
                sampleIntervalMicros = parseDurationMicros(value);
            } else if (key == "wait") {
                waitTimeMicros = parseDurationMicros(value);
            }
        }
    }
    
    return std::make_unique<ScopeCommand>(sampleIntervalMicros, waitTimeMicros);
}

