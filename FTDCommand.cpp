#include "FTDCommand.h"
#include "FTDController.h"

void StartCommand::execute(FTDController* controller) {
    if (controller) {
        controller->startOscilloscope();
    }
}

void StopCommand::execute(FTDController* controller) {
    if (controller) {
        controller->stopOscilloscope();
    }
}

void ReadCommand::execute(FTDController* controller) {
    if (controller) {
        controller->readBytes(count);
    }
}

void WriteCommand::execute(FTDController* controller) {
    if (controller) {
        controller->writeBytes(byteValue, count);
    }
}

void SamplesCommand::execute(FTDController* controller) {
    if (controller) {
        controller->configureSampling(numberOfSamples, intervalMicros);
    }
}

void ReadFileCommand::execute(FTDController* controller) {
    if (controller) {
        controller->readFromFile(filename);
    }
}

void WriteFileCommand::execute(FTDController* controller) {
    if (controller) {
        controller->writeToFile(filename);
    }
}

void ScopeCommand::execute(FTDController* controller) {
    if (controller) {
        controller->runScopeWithWait(sampleIntervalMicros, waitTimeMicros);
    }
}

