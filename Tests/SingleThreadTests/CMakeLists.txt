cmake_minimum_required(VERSION 3.10)
project(singlethread_tests VERSION 1.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
enable_testing(true)

get_filename_component(PROJECT_BASE_DIR ../../ ABSOLUTE)
include_directories(${PROJECT_BASE_DIR})

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.8_2")
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Test)

set(
    CPP_SOURCE_FILES
    main.cpp
    singlethread.tests.h
    ${PROJECT_BASE_DIR}/qcircularbuffer.cpp
)


add_executable(
    ${PROJECT_NAME}
    ${CPP_SOURCE_FILES}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Test)
