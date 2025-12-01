#include "FTDRunMenu.h"
#include "../Controller/FTDController.h"
#include "../Model/FTDReader.h"
#include "../Model/FTDWriter.h"
#include "../Model/FTDShift.h"
#include "../Model/FTDScale.h"
#include <iostream>
#include <memory>

FTDRunMenu::FTDRunMenu(FTDController* controller) : controller(controller) {
    if (!controller) {
        throw std::runtime_error("FTDRunMenu: controller must not be null");
    }
}

void FTDRunMenu::run() {
    int choice = 0;
    while (true) {
        std::cout << "\nControl Menu\n"
                  << "1. Write byte to port\n"
                  << "2. Read byte from port\n"
                  << "3. Driver Test\n"
                  << "4. Add Shift filter\n"
                  << "5. Add Scale filter\n"
                  << "6. Run Pipe\n"
                  << "7. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 7) break;

        switch (choice) {
            case 1: {
                FTDWriter* writer = controller->getWriter();
                if (writer) {
                    writer->write();
                }
                break;
            }
            case 2: {
                FTDReader* reader = controller->getReader();
                if (reader) {
                    reader->read();
                    reader->getBuffer();
                }
                break;
            }
            case 3:
                controller->driverTest();
                break;
            case 4: { // Shift
                int offset;
                std::cout << "Enter shift offset: ";
                std::cin >> offset;
                controller->addProcess(std::make_shared<Shift>(offset));
                std::cout << "Added Shift filter (" << offset << ")\n";
                break;
            }
            case 5: { // Scale
                int factor;
                std::cout << "Enter scale factor: ";
                std::cin >> factor;
                controller->addProcess(std::make_shared<Scale>(factor));
                std::cout << "Added Scale filter (" << factor << ")\n";
                break;
            }
            case 6: {
                controller->executePipe();
                break;
            }
            default:
                std::cout << "Invalid choice. Try again.\n";
                break;
        }
    }
}

