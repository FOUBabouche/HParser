cmake_minimum_required(VERSION 3.3)

project(HParseur)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME})

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE 
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)