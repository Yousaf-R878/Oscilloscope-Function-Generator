#include <FTDController.h>
#include "FTDCommandParser.h"
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <sstream>

// Check if a file exists
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

int main(int argc, char* argv[]) {
    FTDController controller;
    
    // Check if arguments were provided
    if (argc > 1) {
        std::string firstArg = argv[1];
        
        // Check if first argument is a file (exists and likely a command file)
        if (fileExists(firstArg)) {
            // Execute commands from file
            std::cout << "Executing commands from file: " << firstArg << std::endl;
            
            try {
                auto commands = FTDCommandParser::parseFile(firstArg);
                controller.executeCommands(commands);
                std::cout << "Command execution completed." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error executing commands: " << e.what() << std::endl;
                return 1;
            }
        }
        else {
            // Treat all arguments as commands
            std::cout << "Executing commands from command line..." << std::endl;
            
            try {
                // Join all arguments into a single string (handles comma-separated commands)
                std::string cmdLine;
                for (int i = 1; i < argc; i++) {
                    if (i > 1) cmdLine += " ";
                    cmdLine += argv[i];
                }
                
                // Parse commands - handle comma-separated format
                std::vector<std::unique_ptr<FTDCommand>> commands;
                
                // Check if this is a scope command (contains "scope")
                std::string lowerCmdLine = cmdLine;
                std::transform(lowerCmdLine.begin(), lowerCmdLine.end(), lowerCmdLine.begin(), ::tolower);
                
                if (lowerCmdLine.find("scope") != std::string::npos) {
                    // Parse scope command as a single command (handles commas internally)
                    auto cmd = FTDCommandParser::parseCommand(cmdLine);
                    if (cmd) {
                        commands.push_back(std::move(cmd));
                    }
                } else {
                    // Split by commas first, then parse each part
                    std::istringstream iss(cmdLine);
                    std::string segment;
                    std::vector<std::string> segments;
                    
                    while (std::getline(iss, segment, ',')) {
                        segment.erase(0, segment.find_first_not_of(" \t"));
                        segment.erase(segment.find_last_not_of(" \t") + 1);
                        if (!segment.empty()) {
                            segments.push_back(segment);
                        }
                    }
                    
                    // If no commas found, treat as space-separated
                    if (segments.empty()) {
                        segments.push_back(cmdLine);
                    }
                    
                    // Parse each segment
                    for (const auto& seg : segments) {
                        auto cmd = FTDCommandParser::parseCommand(seg);
                        if (cmd) {
                            commands.push_back(std::move(cmd));
                        }
                    }
                }
                
                // Execute all commands
                controller.executeCommands(commands);
                
                std::cout << "Command execution completed." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error executing command: " << e.what() << std::endl;
                return 1;
            }
        }
    }
    else {
        // No arguments provided, run interactive menu
        controller.runMenu();
    }
    
    return 0;
}