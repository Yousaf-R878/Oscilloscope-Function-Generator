#pragma once

class Data {
private:
    int value;
public:
    Data(int v = 0) : value(v) {}
    int getValue() const { return value; }
    void setValue(int v) { value = v; }
};
