cmake_minimum_required(VERSION 3.16)
project(Task1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---> THIS IS THE MAGIC LINE THAT FIXES THE ERROR <---
set(CMAKE_AUTOUIC_SEARCH_PATHS "src/frontend/ui")

# 1. FORCE CMake to look in system directories FIRST, not FSL
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu;/usr")
set(CMAKE_IGNORE_PATH "/home/sandy/fsl/lib;/home/sandy/fsl/bin")

# 2. Find packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

# 3. Explicitly find the SYSTEM versions of curl and tiff
find_library(CURL_LIB NAMES curl PATHS "/usr/lib/x86_64-linux-gnu" NO_DEFAULT_PATH)
find_library(TIFF_LIB NAMES tiff PATHS "/usr/lib/x86_64-linux-gnu" NO_DEFAULT_PATH)

# 4. Collect files
file(GLOB_RECURSE CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE UI_FILES     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui")

# 5. Create executable
add_executable(Task1
    ${CPP_SOURCES}
    ${HEADER_FILES}
    ${UI_FILES}
)

# 6. Include paths
target_include_directories(Task1 PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${OpenCV_INCLUDE_DIRS}
)

# 7. Link everything - notice we put the system libs FIRST
target_link_libraries(Task1 PRIVATE
    ${CURL_LIB}
    ${TIFF_LIB}
    Qt5::Widgets
    ${OpenCV_LIBS}
)