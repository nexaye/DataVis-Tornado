cmake_minimum_required(VERSION 3.22)
project(tornado_visualization)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.7.3/msvc2022_64/lib/cmake/Qt6")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src/")

include_directories(${SRC_DIR})
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_definitions(-DSHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/")
add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/assets/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(tornado-visualization ${SOURCES})
target_link_libraries(tornado-visualization Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)

add_custom_command(TARGET tornado-visualization POST_BUILD
    COMMAND ${Qt6_DIR}/../../../bin/windeployqt6.exe $<TARGET_FILE:tornado-visualization>
    COMMENT "Running windeployqt6 to gather all necessary DLLs"
)