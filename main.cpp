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
                // Join all arguments into a single string
                std::string cmdLine;
                for (int i = 1; i < argc; i++) {
                    if (i > 1) cmdLine += " ";
                    cmdLine += argv[i];
                }
                
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
                    // Original space-separated command parsing logic
                    for (int i = 1; i < argc; ) {
                        std::string cmdLine;
                        std::string cmdName = argv[i];
                        std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(), ::tolower);
                        
                        // Build the command line for this command
                        cmdLine = argv[i];
                        
                        // Determine how many arguments this command needs
                        int argsNeeded = 0;
                        if (cmdName == "read") {
                            argsNeeded = 1;
                        } else if (cmdName == "samples") {
                            argsNeeded = 2; // count + interval
                        } else if (cmdName == "write") {
                            // Write needs at least 1 argument (byte), optionally 2 (byte and count)
                            argsNeeded = 1; // At least byte value
                            if (i + 2 < argc) {
                                // Check if third arg is a number (could be count) or a command name
                                std::string thirdArg = argv[i + 2];
                                std::transform(thirdArg.begin(), thirdArg.end(), thirdArg.begin(), ::tolower);
                                // Check if it's a known command name
                                if (thirdArg != "start" && thirdArg != "stop" && 
                                    thirdArg != "read" && thirdArg != "write" && 
                                    thirdArg != "samples" && thirdArg != "readfile" && 
                                    thirdArg != "writefile" && thirdArg != "scope") {
                                    // Not a command, might be count - try to parse as number
                                    try {
                                        std::stoi(thirdArg);
                                        argsNeeded = 2; // Has both byte and count
                                    } catch (...) {
                                        // Not a number either, might be hex or invalid
                                        argsNeeded = 1; // Just take byte value
                                    }
                                }
                                // If it is a command name, argsNeeded stays 1
                            }
                        } else if (cmdName == "readfile" || cmdName == "writefile") {
                            argsNeeded = 1; // These commands need 1 argument (filename)
                        }
                        // start, stop, and scope need no arguments

                        if (argsNeeded > 0 && (i + argsNeeded >= argc)) {
                            throw std::runtime_error("Insufficient arguments provided for command: " + cmdName);
                        }
                        
                        // Add arguments to command line
                        for (int j = 1; j <= argsNeeded && i + j < argc; j++) {
                            cmdLine += " ";
                            cmdLine += argv[i + j];
                        }
                        
                        // Parse and add command
                        auto cmd = FTDCommandParser::parseCommand(cmdLine);
                        if (cmd) {
                            commands.push_back(std::move(cmd));
                        }
                        
                        // Move to next command
                        i += 1 + argsNeeded;
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