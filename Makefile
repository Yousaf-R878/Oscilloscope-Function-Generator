CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.
LDFLAGS = ./libftd2xx.a -lpthread -framework IOKit -framework CoreFoundation
SRC = main.cpp FTDController.cpp FTDWriter.cpp FTDReader.cpp FTDMorseCode.cpp FTDLEDController.cpp
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -w

clean:
	rm -f $(TARGET)
