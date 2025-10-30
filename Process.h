#include <Data.h>

class Process {
public:
    virtual void execute(Data& data) = 0;
    virtual ~Process() = default;
};
