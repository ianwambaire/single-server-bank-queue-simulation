cmake_minimum_required(VERSION 3.16)

project(BankQueueSimulationGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(BankQueueSimulationGUI
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/QueueSimulator.cpp
    src/CSVHandler.cpp
)

target_link_libraries(BankQueueSimulationGUI Qt6::Widgets)