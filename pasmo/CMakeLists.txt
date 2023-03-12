cmake_minimum_required (VERSION 3.0 FATAL_ERROR)
project (pasmo CXX)
set(TARGET pasmo)


set(PASMO_VERSION 0.5.5.paged)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVERSION=${PASMO_VERSION} " )
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DPASMO_VERSION")

# Turn off additional debug..
add_definitions(-DNDEBUG)

# Defining this allows legacy Pasmo 0.5.5. IX and IY indices greater than 127; and
# brings back index calculation bugs for negative indices.
#add_definitions(-DOLD_PASMO_IXIY_INDICES)

set(SOURCE_FILES
    asm.cxx
    cpc.cxx	
    pasmo.cxx
    tap.cxx
    asmerror.cxx
    macro.cxx
    pasmotypes.cxx
    token.cxx
    asmfile.cxx
    nullstream.cxx
    spectrum.cxx
    tzx.cxx

    asm.h
    asmfile.h
    macro.h
    pasmotypes.h
    tap.h
    token.h
    asmerror.h
    cpc.h
    nullstream.h
    spectrum.h
    test_protocol.h
    tzx.h

    memmap.cxx
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



