CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.
LDFLAGS = ./libftd2xx.a -lpthread -framework IOKit -framework CoreFoundation
SRC = controller.cpp LED_Project.cpp morse_Project.cpp FTDWriter.cpp FTDReader.cpp
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -w

clean:
	rm -f $(TARGET)
