#pragma once

#include <string>
#include <vector>
#include <memory>
#include "FTDCommand.h"

class FTDCommandParser {
public:
    // Parse a single command line
    static std::unique_ptr<FTDCommand> parseCommand(const std::string& line);
    
    // Parse commands from a file
    static std::vector<std::unique_ptr<FTDCommand>> parseFile(const std::string& filename);
    
    // Parse commands from stdin
    static std::vector<std::unique_ptr<FTDCommand>> parseStdin();
    
private:
    // Helper to split string by whitespace
    static std::vector<std::string> split(const std::string& str);
};

