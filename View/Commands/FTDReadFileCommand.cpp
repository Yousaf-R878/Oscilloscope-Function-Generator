#include "FTDReadFileCommand.h"
#include "../../Controller/FTDController.h"

void FTDReadFileCommand::execute(FTDController* controller) {
    if (controller) {
        controller->readFromFile(filename);
    }
}

