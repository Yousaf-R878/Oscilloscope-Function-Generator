CXX ?= g++
CXXFLAGS := -std=c++17 -O2 -pthread

SOURCES := scpCountdown.cpp scpTimer.cpp scpSum.cpp
TARGET := scpCountdown

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES)

clean:
	rm -f $(TARGET)
