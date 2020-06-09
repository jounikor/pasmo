cmake_minimum_required (VERSION 3.0 FATAL_ERROR)
project (pasmo CXX)
set(TARGET pasmo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} )
#set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} -O3 )

# Turn off additional debug..
add_definitions(-DNDEBUG)

set(SOURCE_FILES
    asm.cpp
    asm.h
    asmfile.cpp
    asmfile.h
    cpc.cpp
    cpc.h
    pasmo.cpp
    pasmotypes.cpp
    pasmotypes.h
    spectrum.cpp
    spectrum.h
    tap.cpp
    tap.h
    token.cpp
    token.h
    trace.cpp
    trace.h
    tzx.cpp
    tzx.h
    memmap.cpp
    memmap.h
)



add_executable(${TARGET} ${SOURCE_FILES})

if (WIN32)
	message(STATUS "Windows system")
	#set(STRIPTARGET ${TARGET}.exe)
else()
	message(STATUS "non-Windows system")
	#set(STRIPTARGET ${TARGET})
endif(WIN32)

if (CMAKE_BUILD_TYPE MATCHES RELEASE)
    message(STATUS "kukkuu pulu")
    add_custom_command(TARGET ${TARGET}
        POST_BUILD 
	COMMAND strip $<TARGET_FILE:${TARGET}>
)
endif (CMAKE_BUILD_TYPE MATCHES RELEASE)



