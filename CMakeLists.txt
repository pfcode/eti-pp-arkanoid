cmake_minimum_required(VERSION 3.19)
project(ETIPPArkanoid)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES src/main.cpp src/render.cpp src/map.cpp)

IF(MSVC)
    set(SDL2_DIR "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.14")
    set(SDL2_INCLUDE_LIBS "${SDL2_DIR}/include")
    set(SDL2_LIBRARIES "${SDL2_DIR}/lib/x86/SDL2.lib" "${SDL2_DIR}/lib/x86/SDL2main.lib")
    set(DLL_LIBRARIES_PATH "${SDL2_DIR}/lib/x86/SDL2.dll")
ELSE()
    find_package(SDL2 REQUIRED)
ENDIF()

include_directories(${SDL2_INCLUDE_LIBS})

IF(MSVC)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
ELSE()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
ENDIF()

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})