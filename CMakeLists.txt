# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.10)
project (Stormbreaker)

find_package(Boost 1.58 REQUIRED)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O2 -Wall -DNDEBUG -LC:/lib")

#include_directories(include /usr/local/include/google/tensorflow)
include_directories(include /usr/local/include)

add_library(mjolnir
    mjolnir/src/mj.cpp
    mjolnir/src/nodo.cpp
    mjolnir/src/input_nodes.cpp
    mjolnir/src/processing_nodes.cpp
    #src/deep_nodes.cpp
    #src/nodo_cara.cpp
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

target_include_directories (mjolnir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/mjolnir/include)
target_include_directories (stormbreaker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (organizar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (mjolnir ${OpenCV_LIBS} boost_system stdc++fs pthread)
target_link_libraries (stormbreaker ${OpenCV_LIBS} mjolnir boost_system stdc++fs)
target_link_libraries (organizar ${OpenCV_LIBS} boost_system stdc++fs)

