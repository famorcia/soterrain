set(soterrain_includes
        ${CMAKE_SOURCE_DIR}/includes/So${Gui}FreeViewer.h
        ${CMAKE_SOURCE_DIR}/includes/chunkedlod/SbChunkedLoDPrimitives.h
        ${CMAKE_SOURCE_DIR}/includes/chunkedlod/SoSimpleChunkedLoDTerrain.h
        ${CMAKE_SOURCE_DIR}/includes/debug.h
        ${CMAKE_SOURCE_DIR}/includes/geomipmapping/SbGeoMipmapPrimitives.h
        ${CMAKE_SOURCE_DIR}/includes/geomipmapping/SoSimpleGeoMipmapTerrain.h
        ${CMAKE_SOURCE_DIR}/includes/profiler/PrProfiler.h
        ${CMAKE_SOURCE_DIR}/includes/profiler/SoProfileGroup.h
        ${CMAKE_SOURCE_DIR}/includes/profiler/SoProfileSceneManager.h
        ${CMAKE_SOURCE_DIR}/includes/roam/SbROAMMergeQueue.h
        ${CMAKE_SOURCE_DIR}/includes/roam/SbROAMPrimitives.h
        ${CMAKE_SOURCE_DIR}/includes/roam/SbROAMSplitQueue.h
        ${CMAKE_SOURCE_DIR}/includes/roam/SoSimpleROAMTerrain.h
        ${CMAKE_SOURCE_DIR}/includes/utils.h
        )

set(soterrain_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/chunkedlod/SbChunkedLoDPrimitives.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/chunkedlod/SoSimpleChunkedLoDTerrain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geomipmapping/SbGeoMipmapPrimitives.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geomipmapping/SoSimpleGeoMipmapTerrain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/profiler/PrProfiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/profiler/SoProfileGroup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/profiler/SoProfileSceneManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/roam/SbROAMMergeQueue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/roam/SbROAMPrimitives.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/roam/SbROAMSplitQueue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/roam/SoSimpleROAMTerrain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
        )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SoGuiFreeViewer.cpp.in ${CMAKE_BINARY_DIR}/So${Gui}FreeViewer.cpp @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SoTerrainTest.cpp.in ${CMAKE_BINARY_DIR}/SoTerrainTest.cpp @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/include/SoGuiFreeViewer.h.in ${CMAKE_BINARY_DIR}/So${Gui}FreeViewer.h @ONLY)

add_library(soterrain ${soterrain_srcs})
target_include_directories(soterrain PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(SoTerrainTest
        ${CMAKE_BINARY_DIR}/So${Gui}FreeViewer.cpp
        ${CMAKE_BINARY_DIR}/SoTerrainTest.cpp
        )

target_include_directories(SoTerrainTest PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR})


target_link_libraries(SoTerrainTest soterrain Coin::Coin So${Gui}::So${Gui} simage OpenGL::GL)