#include "FTDCommandParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

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
            throw std::runtime_error("Write command requires a count argument");
        }
        int count = std::stoi(tokens[1]);
        return std::make_unique<WriteCommand>(count);
    }
    else if (command == "samples") {
        if (tokens.size() < 2) {
            throw std::runtime_error("Samples command requires a number argument");
        }
        int numberOfSamples = std::stoi(tokens[1]);
        return std::make_unique<SamplesCommand>(numberOfSamples);
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
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

