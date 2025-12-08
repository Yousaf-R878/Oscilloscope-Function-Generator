CXX = g++
CXXFLAGS_BASE = -g -std=c++17 -Wall -I. -fPIC
LDFLAGS_BASE = ./libftd2xx.a -lpthread -framework IOKit -framework CoreFoundation

# Check if Qt is available (only for scope target)
QT_AVAILABLE := $(shell pkg-config --exists Qt6Core Qt6Widgets 2>/dev/null && echo "yes" || echo "no")
ifeq ($(QT_AVAILABLE),yes)
    QT_CXXFLAGS := $(shell pkg-config --cflags Qt6Core Qt6Widgets)
    QT_LDFLAGS := $(shell pkg-config --libs Qt6Core Qt6Widgets)
    # Add base include path for <QObject> style includes
    QT_CXXFLAGS += -I/opt/homebrew/include
else
    # Try Qt5
    QT5_AVAILABLE := $(shell pkg-config --exists Qt5Core Qt5Widgets 2>/dev/null && echo "yes" || echo "no")
    ifeq ($(QT5_AVAILABLE),yes)
        QT_CXXFLAGS := $(shell pkg-config --cflags Qt5Core Qt5Widgets)
        QT_LDFLAGS := $(shell pkg-config --libs Qt5Core Qt5Widgets)
        # Add base include path
        QT_CXXFLAGS += -I/opt/homebrew/include
    else
        # Try to find Qt via qmake
        QMAKE := $(shell which qmake 2>/dev/null)
        ifneq ($(QMAKE),)
            QT_DIR := $(shell qmake -query QT_INSTALL_PREFIX 2>/dev/null)
            ifneq ($(QT_DIR),)
                # Add base include path for <QObject> style includes
                QT_CXXFLAGS := -I$(QT_DIR)/include -DQT_AVAILABLE
                QT_LDFLAGS := -L$(QT_DIR)/lib -lQt6Core -lQt6Widgets -lQt6Gui
            endif
        endif
        # If still not found, try Homebrew default locations
        ifeq ($(QT_CXXFLAGS),)
            ifeq ($(shell test -d /opt/homebrew/include/QtCore && echo "yes"),yes)
                QT_CXXFLAGS := -I/opt/homebrew/include -DQT_AVAILABLE
                QT_LDFLAGS := -L/opt/homebrew/lib -lQt6Core -lQt6Widgets -lQt6Gui
            else ifeq ($(shell test -d /opt/homebrew/opt/qt@6/include/QtCore && echo "yes"),yes)
                QT_CXXFLAGS := -I/opt/homebrew/opt/qt@6/include -DQT_AVAILABLE
                QT_LDFLAGS := -L/opt/homebrew/opt/qt@6/lib -lQt6Core -lQt6Widgets -lQt6Gui
            endif
        endif
    endif
endif

# Original main target (CLI only, no Qt)
SRC_MAIN = main.cpp \
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

# Scope target (with Qt GUI/CLI views)
SRC_SCOPE = main_scope.cpp \
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
      View/Commands/FTDScopeCommand.cpp \
      View/scpFTDISource.cpp \
      View/scpScopeView.cpp \
      View/scpViewTerminal.cpp \
      View/scpMainWindow.cpp

TARGET_MAIN = main
TARGET_SCOPE = scope

all: $(TARGET_MAIN)

main: $(SRC_MAIN)
	$(CXX) $(CXXFLAGS_BASE) $(SRC_MAIN) -o $(TARGET_MAIN) $(LDFLAGS_BASE) -w

scope: $(SRC_SCOPE)
	@if [ -z "$(QT_CXXFLAGS)" ]; then \
		echo "Error: Qt not found. Install Qt6 or Qt5 to build scope target."; \
		echo "You can install Qt via: brew install qt@6 (or qt@5)"; \
		exit 1; \
	fi
	@echo "Building scope with Qt flags: $(QT_CXXFLAGS)"
	$(CXX) $(CXXFLAGS_BASE) $(QT_CXXFLAGS) $(SRC_SCOPE) -o $(TARGET_SCOPE) $(LDFLAGS_BASE) $(QT_LDFLAGS) -w

clean:
	rm -f $(TARGET_MAIN) $(TARGET_SCOPE)