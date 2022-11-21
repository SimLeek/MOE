cmake_minimum_required(VERSION 3.16)
project(MOE)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set (CMAKE_CXX_STANDARD 17)
add_compile_definitions(COMPILING_DLL=1)

option(MOE_BUILD_TESTS "Build tests for MOE" OFF)


find_package(OpenCV 4.4)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV > 4.4 not found.")
endif()
find_package(cppzmq REQUIRED)
find_package(Flatbuffers REQUIRED)


include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${CPPZMQ_INCLUDE_DIRS}
${OpenCV_INCLUDE_DIR}
${FLATBUFFERS_INCLUDE_DIR}
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

file(GLOB FBUF_HEADERS ${PROJECT_SOURCE_DIR}/include/flatbuffers/*.h)

# glob isn't recommended for source/include files, but flatbuffers change often, so using it for that.
add_executable(${PROJECT_NAME}_main
src/orb_main.cc
src/orb_computer.cc
src/orb_io.cc
include/MOE/orb_computer.h
include/MOE/orb_io.h
include/MOE/MultiPlatform.h
${FBUF_HEADERS})

add_library(${PROJECT_NAME} SHARED 
src/orb_main.cc
src/orb_computer.cc
src/orb_io.cc
include/MOE/orb_computer.h
include/MOE/orb_io.h
include/MOE/MultiPlatform.h
${FBUF_HEADERS})


target_link_libraries(${PROJECT_NAME}_main PRIVATE
${OpenCV_LIBS}
flatbuffers::flatbuffers
cppzmq
)

target_link_libraries(${PROJECT_NAME} PRIVATE
${OpenCV_LIBS}
flatbuffers::flatbuffers
cppzmq
)

configure_file(src/moe.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)

install(TARGETS ${PROJECT_NAME}_main ${PROJECT_NAME}_main DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})


install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}/)

# todo: install with dependencies to make it portable
#  https://stackoverflow.com/questions/62884439/how-to-use-cmake-file-get-runtime-dependencies-in-an-install-statement
# https://stackoverflow.com/questions/60038427/cmake-get-runtime-dependencies-couldnt-find-dll-library-when-it-is-linked-throu/70017008#70017008

if (${MOE_BUILD_TESTS})
    enable_testing()
    add_subdirectory(test)
endif()