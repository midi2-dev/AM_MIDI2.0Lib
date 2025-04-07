cmake_minimum_required(VERSION 3.12)

project(libmidi2)

function(setupTarget target)
    target_compile_options("${target}" PRIVATE -Wall)
    target_compile_features("${target}" PUBLIC cxx_std_11)
endfunction()

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LIBMIDI2_HEADERS
    "${INCLUDE_DIR}/bytestreamToUMP.h"
    "${INCLUDE_DIR}/mcoded7.h"
    "${INCLUDE_DIR}/midiCIMessageCreate.h"
    "${INCLUDE_DIR}/midiCIProcessor.h"
    "${INCLUDE_DIR}/umpMessageCreate.h"
    "${INCLUDE_DIR}/umpProcessor.h"
    "${INCLUDE_DIR}/umpToBytestream.h"
    "${INCLUDE_DIR}/umpToMIDI1Protocol.h"
    "${INCLUDE_DIR}/umpToMIDI2Protocol.h"
    "${INCLUDE_DIR}/utils.h"
)

add_library(libmidi2 ${LIBMIDI2_HEADERS}
        src/umpMessageCreate.cpp
        src/midiCIMessageCreate.cpp
        src/midiCIProcessor.cpp
        src/umpProcessor.cpp
        src/mcoded7.cpp
        )
setupTarget(libmidi2)

target_include_directories(libmidi2 PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set(LIBMIDI2_EXPORT_NAME libmidi2-config)
install(TARGETS libmidi2
        EXPORT ${LIBMIDI2_EXPORT_NAME}
)
install(EXPORT ${LIBMIDI2_EXPORT_NAME}
        NAMESPACE libmidi2::
        DESTINATION "share/${PROJECT_NAME}"
)

install(FILES ${LIBMIDI2_HEADERS} DESTINATION "include/${PROJECT_NAME}")

add_executable(test tests/tests.cpp)
target_link_libraries(test PRIVATE libmidi2)
setupTarget(test)
