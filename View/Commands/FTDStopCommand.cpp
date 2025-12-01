#include "FTDStopCommand.h"
#include "../../Controller/FTDController.h"

void FTDStopCommand::execute(FTDController* controller) {
    if (controller) {
        controller->stopOscilloscope();
    }
}

