# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.5)
project (Stormbreaker)

find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#include_directories(include /usr/local/include/google/tensorflow)
include_directories(include /usr/local/include)

# Add executable called "stormbreaker" that is built from the source files
# "main.cpp" and "*.cxx". The extensions are automatically found.
add_executable (stormbreaker
    main.cpp
    src/mj.cpp
    src/node.cpp
    #src/tensor_mk.cpp
    src/caffe_mk.cpp
    src/dlib_mk.cpp
    src/resnet.cpp
    src/sistema_archivos.cpp)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (stormbreaker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#Finally, in the ./Demo directory, the third and final CMakeLists.txt file is created:



# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
#target_link_libraries (stormbreaker ${OpenCV_LIBS} tensorflow_cc tensorflow_framework dlib::dlib caffe protobuf glog boost_system stdc++fs)
target_link_libraries (stormbreaker ${OpenCV_LIBS} dlib::dlib caffe protobuf glog boost_system stdc++fs)


