# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.5)
project (Stormbreaker)

find_package(Boost 1.58 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Protobuf 3.6.0 REQUIRED )
find_package(dlib REQUIRED)
find_package(glog REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O2 -Wall -DNDEBUG -LC:/lib")

#include_directories(include /usr/local/include/google/tensorflow)
include_directories(include /usr/local/include)

add_library(ml
    src/resnet.cpp
    src/clusters.cpp
    src/detector_facial.cpp
    src/dlib_mk.cpp)

add_library(mjolnir
    src/mj.cpp
    src/nodo.cpp
    src/input_nodes.cpp
    src/processing_nodes.cpp
    #src/deep_nodes.cpp
    src/nodo_cara.cpp
)

# Add executable called "stormbreaker" that is built from the source files
# "main.cpp" and "*.cxx". The extensions are automatically found.
add_executable (stormbreaker
    main.cpp
    #src/tensor_mk.cpp
    #src/caffe_mk.cpp
    utils/sistema_archivos.cpp)

add_executable(organizar
    utils/organizar.cpp
    utils/sistema_archivos.cpp
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(stormbreaker ${Boost_LIBRARIES})
    target_link_libraries(organizar ${Boost_LIBRARIES})
endif()

target_include_directories (ml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (mjolnir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (stormbreaker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (organizar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#target_link_libraries (stormbreaker ${OpenCV_LIBS} tensorflow_cc tensorflow_framework dlib::dlib caffe protobuf glog boost_system stdc++fs)
#<caffe> is a linkable library. We're not using it until we figure how to do SSD

IF (WIN32)
target_link_libraries (ml ${OpenCV_LIBS} protobuf dlib::dlib glog boost_system boost_filesystem)
target_link_libraries (mjolnir ${OpenCV_LIBS} ml protobuf glog boost_system boost_filesystem)
target_link_libraries (stormbreaker ${OpenCV_LIBS} mjolnir protobuf boost_system boost_filesystem dlib::dlib)
target_link_libraries (organizar ${OpenCV_LIBS} ml protobuf boost_system boost_filesystem dlib::dlib)
ELSE()
target_link_libraries (ml ${OpenCV_LIBS} protobuf dlib::dlib glog boost_system boost_filesystem)
target_link_libraries (mjolnir ${OpenCV_LIBS} ml protobuf glog boost_system stdc++fs)
target_link_libraries (stormbreaker ${OpenCV_LIBS} mjolnir protobuf dlib::dlib glog boost_system stdc++fs)
target_link_libraries (organizar ${OpenCV_LIBS} ml protobuf dlib::dlib glog boost_system stdc++fs)
ENDIF()

