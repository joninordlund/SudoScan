cmake_minimum_required(VERSION 3.16)
project(SudoScan LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

if(ipo_supported)
    message(STATUS "LTO (Link Time Optimization) supported – turn on for Release/RelWithDebInfo builds")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
else()
    message(STATUS "LTO not in use: ${ipo_error}")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g")
    
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -O3")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -O3")
   
endif()

add_subdirectory(Gui)
add_subdirectory(reader)
add_subdirectory(solver)