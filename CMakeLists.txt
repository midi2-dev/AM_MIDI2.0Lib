cmake_minimum_required(VERSION 3.12)

project(libmidi2)
set(CMAKE_CXX_STANDARD 17)

add_library(libmidi2
        src/utils.cpp
        src/bytestreamToUMP.cpp
        src/umpToBytestream.cpp
        src/umpMessageCreate.cpp
        src/midiCIMessageCreate.cpp
        src/midiCIProcessor.cpp
        src/umpProcessor.cpp
        src/mcoded7.cpp
        )

target_include_directories(libmidi2 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(libmidi2 PRIVATE -Wall)

install(TARGETS libmidi2 DESTINATION lib)
file(GLOB HEADERS include/*.h)
install(FILES ${HEADERS} DESTINATION "include/${PROJECT_NAME}")
