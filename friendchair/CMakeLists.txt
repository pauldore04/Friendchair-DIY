cmake_minimum_required(VERSION 3.16)
project(friendchair)

# Set the toolchain for ESP32
set(CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32/cmake/toolchain-esp32.cmake)

# Include PlatformIO CMake integration
include($ENV{HOME}/.platformio/packages/contrib-piohome/misc/cmake/platformio.cmake)

# Add PlatformIO targets
add_platformio_target()

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.c"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})