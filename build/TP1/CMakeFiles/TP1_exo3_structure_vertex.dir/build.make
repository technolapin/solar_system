# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/clement/Documents/synthese/GLImac-Template

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/clement/Documents/synthese/build

# Include any dependencies generated for this target.
include TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/depend.make

# Include the progress variables for this target.
include TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/progress.make

# Include the compile flags for this target's objects.
include TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/flags.make

TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.o: TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/flags.make
TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.o: /home/clement/Documents/synthese/GLImac-Template/TP1/exo3_structure_vertex.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/clement/Documents/synthese/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.o"
	cd /home/clement/Documents/synthese/build/TP1 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.o -c /home/clement/Documents/synthese/GLImac-Template/TP1/exo3_structure_vertex.cpp

TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.i"
	cd /home/clement/Documents/synthese/build/TP1 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/clement/Documents/synthese/GLImac-Template/TP1/exo3_structure_vertex.cpp > CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.i

TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.s"
	cd /home/clement/Documents/synthese/build/TP1 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/clement/Documents/synthese/GLImac-Template/TP1/exo3_structure_vertex.cpp -o CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.s

# Object files for target TP1_exo3_structure_vertex
TP1_exo3_structure_vertex_OBJECTS = \
"CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.o"

# External object files for target TP1_exo3_structure_vertex
TP1_exo3_structure_vertex_EXTERNAL_OBJECTS =

TP1/TP1_exo3_structure_vertex: TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/exo3_structure_vertex.cpp.o
TP1/TP1_exo3_structure_vertex: TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/build.make
TP1/TP1_exo3_structure_vertex: glimac/libglimac.a
TP1/TP1_exo3_structure_vertex: /usr/lib/libSDLmain.a
TP1/TP1_exo3_structure_vertex: /usr/lib/libSDL.so
TP1/TP1_exo3_structure_vertex: /usr/lib/libGL.so
TP1/TP1_exo3_structure_vertex: /usr/lib/libGLU.so
TP1/TP1_exo3_structure_vertex: /usr/lib/libGLEW.so
TP1/TP1_exo3_structure_vertex: TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/clement/Documents/synthese/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TP1_exo3_structure_vertex"
	cd /home/clement/Documents/synthese/build/TP1 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TP1_exo3_structure_vertex.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/build: TP1/TP1_exo3_structure_vertex

.PHONY : TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/build

TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/clean:
	cd /home/clement/Documents/synthese/build/TP1 && $(CMAKE_COMMAND) -P CMakeFiles/TP1_exo3_structure_vertex.dir/cmake_clean.cmake
.PHONY : TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/clean

TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/depend:
	cd /home/clement/Documents/synthese/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/clement/Documents/synthese/GLImac-Template /home/clement/Documents/synthese/GLImac-Template/TP1 /home/clement/Documents/synthese/build /home/clement/Documents/synthese/build/TP1 /home/clement/Documents/synthese/build/TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : TP1/CMakeFiles/TP1_exo3_structure_vertex.dir/depend

