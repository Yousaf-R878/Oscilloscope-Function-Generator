#include "Controller/FTDController.h"
#include "View/FTDFTDISource.h"
#include "View/FTDMainWindow.h"
#include "View/FTDViewTerminal.h"
#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    // Parse command line arguments to determine mode
    bool useGui = false;
    bool useCli = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--ui" || arg == "--gui") {
            useGui = true;
        } else if (arg == "--cli") {
            useCli = true;
        }
    }
    
    // Default to CLI if no flag specified
    if (!useGui && !useCli) {
        useCli = true;
    }
    
    // Create appropriate Qt application
    std::unique_ptr<QCoreApplication> coreApp;
    std::unique_ptr<QApplication> guiApp;
    
    if (useGui) {
        guiApp = std::make_unique<QApplication>(argc, argv);
    } else {
        coreApp = std::make_unique<QCoreApplication>(argc, argv);
    }
    
    QCoreApplication& app = useGui ? static_cast<QCoreApplication&>(*guiApp) : *coreApp;
    
    // Create controller
    FTDController controller;
    
    // Create FTDI source
    FTDFTDISource ftdiSource(&controller);
    
    if (useGui) {
        // GUI mode
        FTDMainWindow mainWindow(&controller);
        mainWindow.resize(1000, 600);
        mainWindow.show();
        
        return app.exec();
    } else {
        // CLI mode
        FTDViewTerminal terminalView;
        terminalView.setSource(&ftdiSource);
        terminalView.setController(&controller);
        terminalView.setTotalTimeWindowSec(0.5);  // 500ms across screen
        terminalView.setVerticalScale(1.0f);
        
        std::cout << "Oscilloscope Terminal View" << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
        std::cout << "Type 'scope start' to begin sampling" << std::endl;
        
        terminalView.start();
        
        return app.exec();
    }
}

