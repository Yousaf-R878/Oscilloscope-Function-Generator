#include <Process.h>
class Scale : public Process {
    int factor;
public:
    Scale(int f) : factor(f) {}
    void execute(Data& data) override {
        int val = data.getValue();
        data.setValue(val * factor);
    }
};
