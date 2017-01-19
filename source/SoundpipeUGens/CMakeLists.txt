cmake_minimum_required (VERSION 2.8)
set(PROJECT "Padsynth")
set(SOUNDPIPEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external_libraries/Soundpipe)
project (${PROJECT})

if(NOT EXISTS ${SOUNDPIPEDIR}/h/base.h)
  message(FATAL_ERROR "Should use git submodule update and git submodule init to get Soundpipe")
endif()

# Soundpipe
include_directories(${SOUNDPIPEDIR})
include_directories(${SOUNDPIPEDIR}/h)
include_directories(${SOUNDPIPEDIR}/modules)
include_directories(${SOUNDPIPEDIR}/lib/kissfft)
link_directories(${SoundpipeLibs})
file(GLOB SoundpipeLibs "${SOUNDPIPEDIR}/lib/kissfft/*.c")
file(GLOB SoundpipeSources
  "${SOUNDPIPEDIR}/modules/base.c"
  "${SOUNDPIPEDIR}/modules/osc.c"
  "${SOUNDPIPEDIR}/modules/randmt.c"
  "${SOUNDPIPEDIR}/modules/ftbl.c"
  "${SOUNDPIPEDIR}/modules/fftwrapper.c"
  "${SOUNDPIPEDIR}/modules/padsynth.c"
  )
add_library(${PROJECT} MODULE ${PROJECT}.cpp ${SoundpipeLibs} ${SoundpipeSources})
# The following explicitly links Padsaw against libsndfile and kiss_fft
target_link_libraries(${PROJECT} sndfile)
#target_link_libraries(${PROJECT} kiss_fft.o)
#target_link_libraries(${PROJECT} kiss_fftr.o)

# SuperCollider
include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/common)

# libsndfile
include_directories(${SC_PATH}/external_libraries/libsndfile/)

# Include libsndfile
#set(SNDFILEDIR /usr/local/Cellar/libsndfile/1.0.26)
#include_directories(${SNDFILEDIR}/include)
#link_directories(${SNDFILEDIR}/lib)

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

option(CPP11 "Build with c++11." ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    add_definitions(-fvisibility=hidden)

    include (CheckCCompilerFlag)
    include (CheckCXXCompilerFlag)

    CHECK_C_COMPILER_FLAG(-msse HAS_SSE)
    CHECK_CXX_COMPILER_FLAG(-msse HAS_CXX_SSE)

    if (HAS_SSE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse")
    endif()
    if (HAS_CXX_SSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
    endif()

    CHECK_C_COMPILER_FLAG(-msse2 HAS_SSE2)
    CHECK_CXX_COMPILER_FLAG(-msse2 HAS_CXX_SSE2)

    if (HAS_SSE2)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
    endif()
    if (HAS_CXX_SSE2)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    endif()

    CHECK_C_COMPILER_FLAG(-mfpmath=sse HAS_FPMATH_SSE)
    CHECK_CXX_COMPILER_FLAG(-mfpmath=sse HAS_CXX_FPMATH_SSE)

    if (HAS_FPMATH_SSE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse")
    endif()
    if (HAS_CXX_FPMATH_SSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")
    endif()

    if(NATIVE)
        add_definitions(-march=native)
    endif()

    if(CPP11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        if(CMAKE_COMPILER_IS_CLANG)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        endif()
    endif()
endif()
if(MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mstackrealign")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mstackrealign")
endif()

install(DIRECTORY "sc/" DESTINATION "${INSTALL_DESTINATION_DISTRO}/${PROJECT}" PATTERN "*")
