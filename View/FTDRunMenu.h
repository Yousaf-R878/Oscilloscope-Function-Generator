#pragma once

// Forward declaration
class FTDController;

class FTDRunMenu {
public:
    FTDRunMenu(FTDController* controller);
    
    // Run the interactive menu
    void run();

private:
    FTDController* controller;
};

