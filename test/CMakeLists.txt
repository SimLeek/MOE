cmake_minimum_required(VERSION 3.12)

project(MOE_test)

add_compile_definitions(COMPILING_DLL=0)


include(CTest)
enable_testing()

find_package(OpenCV 4.4)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV > 4.4 not found.")
endif()
#find_package(ZeroMQ CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(Flatbuffers REQUIRED)

include_directories(
	"../src/"
    "../include/"
)

set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)


#add_executable(com_test
#	com_test.cpp frame_writer.cc doctest.h
#)

#target_link_libraries(com_test PRIVATE
#${OpenCV_LIBS}
#${Boost_LIBRARIES}
#flatbuffers::flatbuffers
#libzmq libzmq-static
#)

#target_compile_features(com_test PRIVATE cxx_std_17)

#add_executable(basic_zmq_test
#    basic_zmq_test.cpp doctest.h
#)

#target_link_libraries(basic_zmq_test PRIVATE
#cppzmq
#)

#target_compile_features(basic_zmq_test PRIVATE cxx_std_17)

#add_test(NAME basic_zmq_test COMMAND basic_zmq_test)

set(A com_bind_test;com_pub_test;com_sub_test)
foreach(X IN LISTS A)
    add_executable(${X}
    ${X}.cpp doctest.h
    )

    target_link_libraries(${X} PRIVATE
    cppzmq
    ${OpenCV_LIBS}
    flatbuffers::flatbuffers
    "../../build/bin/Debug/MOE"
    )

    target_compile_features(${X} PRIVATE cxx_std_17)

    add_test(NAME ${X} COMMAND ${X})
    set_tests_properties(${X} PROPERTIES RUN_SERIAL TRUE)  # only one program can have access to the camera at a time
endforeach()

