# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/mike/Desktop/clion-2019.3.5/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/mike/Desktop/clion-2019.3.5/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mike/proyectos/stormbreaker

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mike/proyectos/stormbreaker/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/organizar.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/organizar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/organizar.dir/flags.make

CMakeFiles/organizar.dir/utils/organizar.cpp.o: CMakeFiles/organizar.dir/flags.make
CMakeFiles/organizar.dir/utils/organizar.cpp.o: ../utils/organizar.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mike/proyectos/stormbreaker/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/organizar.dir/utils/organizar.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/organizar.dir/utils/organizar.cpp.o -c /home/mike/proyectos/stormbreaker/utils/organizar.cpp

CMakeFiles/organizar.dir/utils/organizar.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/organizar.dir/utils/organizar.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mike/proyectos/stormbreaker/utils/organizar.cpp > CMakeFiles/organizar.dir/utils/organizar.cpp.i

CMakeFiles/organizar.dir/utils/organizar.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/organizar.dir/utils/organizar.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mike/proyectos/stormbreaker/utils/organizar.cpp -o CMakeFiles/organizar.dir/utils/organizar.cpp.s

CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.o: CMakeFiles/organizar.dir/flags.make
CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.o: ../utils/sistema_archivos.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mike/proyectos/stormbreaker/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.o -c /home/mike/proyectos/stormbreaker/utils/sistema_archivos.cpp

CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mike/proyectos/stormbreaker/utils/sistema_archivos.cpp > CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.i

CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mike/proyectos/stormbreaker/utils/sistema_archivos.cpp -o CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.s

# Object files for target organizar
organizar_OBJECTS = \
"CMakeFiles/organizar.dir/utils/organizar.cpp.o" \
"CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.o"

# External object files for target organizar
organizar_EXTERNAL_OBJECTS =

organizar: CMakeFiles/organizar.dir/utils/organizar.cpp.o
organizar: CMakeFiles/organizar.dir/utils/sistema_archivos.cpp.o
organizar: CMakeFiles/organizar.dir/build.make
organizar: /usr/local/lib/libopencv_cudabgsegm.so.4.0.0
organizar: /usr/local/lib/libopencv_cudaobjdetect.so.4.0.0
organizar: /usr/local/lib/libopencv_cudastereo.so.4.0.0
organizar: /usr/local/lib/libopencv_dnn.so.4.0.0
organizar: /usr/local/lib/libopencv_ml.so.4.0.0
organizar: /usr/local/lib/libopencv_shape.so.4.0.0
organizar: /usr/local/lib/libopencv_stitching.so.4.0.0
organizar: /usr/local/lib/libopencv_superres.so.4.0.0
organizar: /usr/local/lib/libopencv_videostab.so.4.0.0
organizar: /usr/local/lib/libopencv_viz.so.4.0.0
organizar: /usr/local/lib/libopencv_cudafeatures2d.so.4.0.0
organizar: /usr/local/lib/libopencv_cudacodec.so.4.0.0
organizar: /usr/local/lib/libopencv_cudaoptflow.so.4.0.0
organizar: /usr/local/lib/libopencv_cudalegacy.so.4.0.0
organizar: /usr/local/lib/libopencv_cudawarping.so.4.0.0
organizar: /usr/local/lib/libopencv_objdetect.so.4.0.0
organizar: /usr/local/lib/libopencv_photo.so.4.0.0
organizar: /usr/local/lib/libopencv_cudaimgproc.so.4.0.0
organizar: /usr/local/lib/libopencv_cudafilters.so.4.0.0
organizar: /usr/local/lib/libopencv_cudaarithm.so.4.0.0
organizar: /usr/local/lib/libopencv_video.so.4.0.0
organizar: /usr/local/lib/libopencv_calib3d.so.4.0.0
organizar: /usr/local/lib/libopencv_features2d.so.4.0.0
organizar: /usr/local/lib/libopencv_flann.so.4.0.0
organizar: /usr/local/lib/libopencv_highgui.so.4.0.0
organizar: /usr/local/lib/libopencv_videoio.so.4.0.0
organizar: /usr/local/lib/libopencv_imgcodecs.so.4.0.0
organizar: /usr/local/lib/libopencv_imgproc.so.4.0.0
organizar: /usr/local/lib/libopencv_core.so.4.0.0
organizar: /usr/local/lib/libopencv_cudev.so.4.0.0
organizar: CMakeFiles/organizar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mike/proyectos/stormbreaker/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable organizar"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/organizar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/organizar.dir/build: organizar

.PHONY : CMakeFiles/organizar.dir/build

CMakeFiles/organizar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/organizar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/organizar.dir/clean

CMakeFiles/organizar.dir/depend:
	cd /home/mike/proyectos/stormbreaker/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mike/proyectos/stormbreaker /home/mike/proyectos/stormbreaker /home/mike/proyectos/stormbreaker/cmake-build-release /home/mike/proyectos/stormbreaker/cmake-build-release /home/mike/proyectos/stormbreaker/cmake-build-release/CMakeFiles/organizar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/organizar.dir/depend

