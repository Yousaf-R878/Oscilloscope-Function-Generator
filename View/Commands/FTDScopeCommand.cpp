#include "FTDScopeCommand.h"
#include "../../Controller/FTDController.h"

void FTDScopeCommand::execute(FTDController* controller) {
    if (controller) {
        controller->runScopeWithWait(sampleIntervalMicros, waitTimeMicros);
    }
}

