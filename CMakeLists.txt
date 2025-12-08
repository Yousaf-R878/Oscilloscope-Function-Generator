cmake_minimum_required(VERSION 3.21)
project(OscilloscopeFunctionGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Original main executable (CLI only, no Qt)
add_executable(main
    main.cpp
    Controller/FTDController.cpp
    Controller/FTDController.h
    Model/FTDReader.cpp
    Model/FTDReader.h
    Model/FTDWriter.cpp
    Model/FTDWriter.h
    Model/FTDOscilloscope.cpp
    Model/FTDOscilloscope.h
    Model/FTDOscilloscopeThreaded.cpp
    Model/FTDOscilloscopeThreaded.h
    View/FTDCommandParser.cpp
    View/FTDCommandParser.h
    View/FTDRunMenu.cpp
    View/FTDRunMenu.h
    View/Commands/FTDCommand.cpp
    View/Commands/FTDCommand.h
    View/Commands/FTDStartCommand.cpp
    View/Commands/FTDStartCommand.h
    View/Commands/FTDStopCommand.cpp
    View/Commands/FTDStopCommand.h
    View/Commands/FTDReadCommand.cpp
    View/Commands/FTDReadCommand.h
    View/Commands/FTDWriteCommand.cpp
    View/Commands/FTDWriteCommand.h
    View/Commands/FTDSamplesCommand.cpp
    View/Commands/FTDSamplesCommand.h
    View/Commands/FTDReadFileCommand.cpp
    View/Commands/FTDReadFileCommand.h
    View/Commands/FTDWriteFileCommand.cpp
    View/Commands/FTDWriteFileCommand.h
    View/Commands/FTDScopeCommand.cpp
    View/Commands/FTDScopeCommand.h
)

target_link_libraries(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libftd2xx.a
    pthread
)

# macOS frameworks
if(APPLE)
    target_link_libraries(main PRIVATE
        "-framework IOKit"
        "-framework CoreFoundation"
    )
endif()

# Scope executable (with Qt GUI/CLI views)
add_executable(scope
    main_scope.cpp
    Controller/FTDController.cpp
    Controller/FTDController.h
    Model/FTDReader.cpp
    Model/FTDReader.h
    Model/FTDWriter.cpp
    Model/FTDWriter.h
    Model/FTDOscilloscope.cpp
    Model/FTDOscilloscope.h
    Model/FTDOscilloscopeThreaded.cpp
    Model/FTDOscilloscopeThreaded.h
    View/FTDCommandParser.cpp
    View/FTDCommandParser.h
    View/FTDRunMenu.cpp
    View/FTDRunMenu.h
    View/Commands/FTDCommand.cpp
    View/Commands/FTDCommand.h
    View/Commands/FTDStartCommand.cpp
    View/Commands/FTDStartCommand.h
    View/Commands/FTDStopCommand.cpp
    View/Commands/FTDStopCommand.h
    View/Commands/FTDReadCommand.cpp
    View/Commands/FTDReadCommand.h
    View/Commands/FTDWriteCommand.cpp
    View/Commands/FTDWriteCommand.h
    View/Commands/FTDSamplesCommand.cpp
    View/Commands/FTDSamplesCommand.h
    View/Commands/FTDReadFileCommand.cpp
    View/Commands/FTDReadFileCommand.h
    View/Commands/FTDWriteFileCommand.cpp
    View/Commands/FTDWriteFileCommand.h
    View/Commands/FTDScopeCommand.cpp
    View/Commands/FTDScopeCommand.h
    # Qt view files (MOC will be handled automatically)
    View/FTDDataSource.h
    View/FTDFTDISource.h
    View/FTDFTDISource.cpp
    View/FTDView.h
    View/FTDScopeView.h
    View/FTDScopeView.cpp
    View/FTDViewTerminal.h
    View/FTDViewTerminal.cpp
    View/FTDMainWindow.h
    View/FTDMainWindow.cpp
)

target_link_libraries(scope PRIVATE
    Qt6::Core
    Qt6::Widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/libftd2xx.a
    pthread
)

# macOS frameworks
if(APPLE)
    target_link_libraries(scope PRIVATE
        "-framework IOKit"
        "-framework CoreFoundation"
    )
endif()

