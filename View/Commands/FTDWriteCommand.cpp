#include "FTDWriteCommand.h"
#include "../../Controller/FTDController.h"

void FTDWriteCommand::execute(FTDController* controller) {
    if (controller) {
        controller->writeBytes(byteValue, count);
    }
}

