#include <vector>
#include <memory>
#include "FTDProcess.h"

class Pipe {
    std::vector<std::shared_ptr<Process>> processes;
public:
    void addProcess(std::shared_ptr<Process> p) {
        processes.push_back(p);
    }

    void execute(Data& data) {
        for (auto& process : processes) {
            process->execute(data);
        }
    }
    void clear() { 
        processes.clear(); 
    }
};
