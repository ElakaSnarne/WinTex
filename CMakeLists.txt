cmake_minimum_required(VERSION 3.10)
project(WinTex)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    file(GLOB SOURCES "*.cpp")
    add_definitions(-DPLATFORM_WINDOWS -DUNICODE -D_UNICODE)
    if(DEFINED ENV{DXSDK_DIR})
        include_directories($ENV{DXSDK_DIR}/include)
    endif()
else()
    file(GLOB SOURCES "*.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WinTex.cpp")
    message(STATUS "Sources: ${SOURCES}")
    add_definitions(-DPLATFORM_LINUX)
    
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    
    include_directories(${SDL2_INCLUDE_DIRS})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer")
endif()

add_executable(WinTex ${SOURCES})

if(WIN32)
    if(DEFINED ENV{DXSDK_DIR})
        find_library(D3DX11_LIB NAMES d3dx11 PATHS "$ENV{DXSDK_DIR}/lib/x64" "$ENV{DXSDK_DIR}/lib/x86" NO_DEFAULT_PATH)
        if(D3DX11_LIB)
            target_link_libraries(WinTex ${D3DX11_LIB})
        else()
            link_directories($ENV{DXSDK_DIR}/lib/x64)
            link_directories($ENV{DXSDK_DIR}/lib/x86)
        endif()
    endif()
endif()

if(NOT WIN32)
    target_link_libraries(WinTex m ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
endif()
