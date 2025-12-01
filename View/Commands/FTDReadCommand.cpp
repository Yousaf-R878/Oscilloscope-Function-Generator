#include "FTDReadCommand.h"
#include "../../Controller/FTDController.h"

void FTDReadCommand::execute(FTDController* controller) {
    if (controller) {
        controller->readBytes(count);
    }
}

