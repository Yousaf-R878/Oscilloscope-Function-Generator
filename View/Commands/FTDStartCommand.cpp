#include "FTDStartCommand.h"
#include "../../Controller/FTDController.h"

void FTDStartCommand::execute(FTDController* controller) {
    if (controller) {
        controller->startOscilloscope();
    }
}

