cmake_minimum_required(VERSION 3.10)
project(QCircularBuffer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON) # Commented this out because I want to place my .ui files in a different folder

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.8_1")
set(QT_REQUIRED_PACKAGES Core Gui Widgets Multimedia)
find_package(Qt5 REQUIRED COMPONENTS ${QT_REQUIRED_PACKAGES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(
    CPP_SOURCE_FILES 
	main.cpp 
    qcircularbuffer.cpp
	)
add_executable(
    ${PROJECT_NAME}
    ${CPP_SOURCE_FILES}
    )

set(QT_LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)
target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    )
target_link_libraries(
    ${PROJECT_NAME} PUBLIC 
    ${QT_LINK_LIBRARIES} 
    )
