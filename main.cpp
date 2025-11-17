#include <FTDController.h>
#include "FTDCommandParser.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    FTDController controller;
    
    // Check if a command file was provided
    if (argc > 1) {
        std::string filename = argv[1];
        std::cout << "Executing commands from file: " << filename << std::endl;
        
        try {
            // Parse commands from file
            auto commands = FTDCommandParser::parseFile(filename);
            
            // Execute all commands
            controller.executeCommands(commands);
            
            std::cout << "Command execution completed." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error executing commands: " << e.what() << std::endl;
            return 1;
        }
    }
    else {
        // No file provided, run interactive menu
        controller.runMenu();
    }
    
    return 0;
}