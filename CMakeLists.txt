cmake_minimum_required(VERSION 3.10)
project(ComputerVision_Task1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 1. Tell CMake exactly where the UI files are stored
set(CMAKE_AUTOUIC_SEARCH_PATHS "src/frontend/ui")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
# 2. explicitly grab the UI files too
file(GLOB_RECURSE UI_FILES "src/*.ui")

# 3. Add the UI files to the executable target
add_executable(launch ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(launch Qt5::Widgets ${OpenCV_LIBS})