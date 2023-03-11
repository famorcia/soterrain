bin_PROGRAMS = SoTerrainTest

SoTerrainTest_SOURCES = src/SoTerrainTest.cpp \
                        src/SoQtFreeViewer.cpp include/SoQtFreeViewer.h \
                        src/utils.cpp include/utils.h \
                        include/debug.h
SoTerrainTest_CPPFLAGS = $(AM_CPPFLAGS)
LDADD = libSoTerrain.la libPrProfiler.la

lib_LTLIBRARIES = libSoTerrain.la libPrProfiler.la

libSoTerrain_la_SOURCES = src/roam/SbROAMPrimitives.cpp include/roam/SbROAMPrimitives.h \
                          src/roam/SbROAMSplitQueue.cpp include/roam/SbROAMSplitQueue.h \
                          src/roam/SbROAMMergeQueue.cpp include/roam/SbROAMMergeQueue.h \
                          src/roam/SoSimpleROAMTerrain.cpp include/roam/SoSimpleROAMTerrain.h \
                          src/geomipmapping/SbGeoMipmapPrimitives.cpp include/geomipmapping/SbGeoMipmapPrimitives.h \
                          src/geomipmapping/SoSimpleGeoMipmapTerrain.cpp include/geomipmapping/SoSimpleGeoMipmapTerrain.h \
                          src/chunkedlod/SbChunkedLoDPrimitives.cpp include/chunkedlod/SbChunkedLoDPrimitives.h \
                          src/chunkedlod/SoSimpleChunkedLoDTerrain.cpp include/chunkedlod/SoSimpleChunkedLoDTerrain.h \
                          src/utils.cpp include/utils.h \
                          include/debug.h
libSoTerrain_la_LDFLAGS=-release @SO_TERRAIN_VERSION@


libPrProfiler_la_SOURCES = src/profiler/PrProfiler.cpp include/profiler/PrProfiler.h \
                           src/profiler/SoProfileGroup.cpp include/profiler/SoProfileGroup.h \
                           src/profiler/SoProfileSceneManager.cpp include/profiler/SoProfileSceneManager.h \
                           include/debug.h
libPrProfiler_la_LDFLAGS=-release @PR_PROFILER_VERSION@

EXTRA_DIST = COPYING \
             TODO \
             README \
             autogen.sh \
             doxyfile \
             images/heightmaps/ps_height_1k.png \
             images/textures/ps_texture_1k.jpg \
             doc/chunked_lod.pdf

doc : doxyfile
	doxygen doxyfile
	touch doxyfile

doc-clean :
	rm -rf doc/html/*

run : SoTerrainTest
	LD_LIBRARY_PATH="@SOQT_PATH@/lib:@SIMAGE_PATH@/lib" ./SoTerrainTest -a chunkedlod -h images/heightmaps/ps_height_1k.png -t images/textures/ps_texture_1k.jpg
