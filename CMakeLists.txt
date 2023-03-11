project(SoTerrain)

cmake_minimum_required(VERSION 3.23)

# Add path for Coin specific utility scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
include(CoinCMakeUtilities)
if(NOT Gui)
    set(Gui Wx)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
include(CoinCMakeUtilities)

coin_project(soterrain VERSION 0.5.0 DESCRIPTION "ROAM in Open Inventor(c)")

find_package(OpenGL REQUIRED)

find_package(Coin REQUIRED)

find_package(So${Gui} REQUIRED)

# find_package(simage REQUIRED)

include_directories(BEFORE
        ${So${Gui}_INCLUDE_DIRS}
        ${Coin_INCLUDE_DIR}
        )

MESSAGE(${So${Gui}_INCLUDE_DIRS})
MESSAGE(${Coin_INCLUDE_DIR})

add_subdirectory(src)