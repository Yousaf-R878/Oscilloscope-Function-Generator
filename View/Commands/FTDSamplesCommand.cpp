#include "FTDSamplesCommand.h"
#include "../../Controller/FTDController.h"

void FTDSamplesCommand::execute(FTDController* controller) {
    if (controller) {
        controller->configureSampling(numberOfSamples, intervalMicros);
    }
}

