# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.10)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)

# Add the root of this git repo to the component search path.
set(EXTRA_COMPONENT_DIRS "../")

project(ZWUtils_IDF8266_Tests)
set(CMAKE_CXX_STANDARD 17)
