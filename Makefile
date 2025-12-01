CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.
LDFLAGS = ./libftd2xx.a -lpthread -framework IOKit -framework CoreFoundation
SRC = main.cpp \
      Controller/FTDController.cpp \
      Model/FTDReader.cpp \
      Model/FTDWriter.cpp \
      Model/FTDOscilloscope.cpp \
      Model/FTDOscilloscopeThreaded.cpp \
      View/FTDCommandParser.cpp \
      View/FTDRunMenu.cpp \
      View/Commands/FTDCommand.cpp \
      View/Commands/FTDStartCommand.cpp \
      View/Commands/FTDStopCommand.cpp \
      View/Commands/FTDReadCommand.cpp \
      View/Commands/FTDWriteCommand.cpp \
      View/Commands/FTDSamplesCommand.cpp \
      View/Commands/FTDReadFileCommand.cpp \
      View/Commands/FTDWriteFileCommand.cpp \
      View/Commands/FTDScopeCommand.cpp
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -w

clean:
	rm -f $(TARGET)