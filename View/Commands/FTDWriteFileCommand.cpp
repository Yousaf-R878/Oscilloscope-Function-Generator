#include "FTDWriteFileCommand.h"
#include "../../Controller/FTDController.h"

void FTDWriteFileCommand::execute(FTDController* controller) {
    if (controller) {
        controller->writeToFile(filename);
    }
}

