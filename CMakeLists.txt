cmake_minimum_required(VERSION 3.25)

project(my-diy-reimplemented-tools)
set(CMAKE_CXX_STANDARD 20) 

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)

# set(GRAPHICS GRAPHICS_API_OPENGL_43 CACHE STRING "" FORCE)
# set(OPENGL_VERSION 4.3 CACHE STRING "" FORCE)


set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE CPP_SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HPP_SOURCES "${SRC_DIR}/*.hpp")
set(SOURCES ${CPP_SOURCES} ${HPP_SOURCES})

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src") 
include_directories("${INCLUDE_DIR}")
 

include(FetchContent)

find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)


# Define the project name as a preprocessor macro
add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")