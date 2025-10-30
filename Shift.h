#include <Process.h>
class Shift : public Process {
    int offset;
public:
    Shift(int o) : offset(o) {}
    void execute(Data& data) override {
        int val = data.getValue();
        data.setValue(val + offset);
    }
};
