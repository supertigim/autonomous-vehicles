# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.8

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Work\autonomous-vehicles\opengl-env

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Work\autonomous-vehicles\opengl-env\win_build

# Include any dependencies generated for this target.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/depend.make

# Include the progress variables for this target.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/progress.make

# Include the compile flags for this target's objects.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/flags.make

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/flags.make
external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/includes_C.rsp
external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj: ../external/glfw-3.1.2/examples/gears.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Work\autonomous-vehicles\opengl-env\win_build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj"
	cd /d C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\gears.dir\gears.c.obj   -c C:\Work\autonomous-vehicles\opengl-env\external\glfw-3.1.2\examples\gears.c

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gears.dir/gears.c.i"
	cd /d C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Work\autonomous-vehicles\opengl-env\external\glfw-3.1.2\examples\gears.c > CMakeFiles\gears.dir\gears.c.i

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gears.dir/gears.c.s"
	cd /d C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Work\autonomous-vehicles\opengl-env\external\glfw-3.1.2\examples\gears.c -o CMakeFiles\gears.dir\gears.c.s

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.requires:

.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.requires

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.provides: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.requires
	$(MAKE) -f external\glfw-3.1.2\examples\CMakeFiles\gears.dir\build.make external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.provides.build
.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.provides

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.provides.build: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj


# Object files for target gears
gears_OBJECTS = \
"CMakeFiles/gears.dir/gears.c.obj"

# External object files for target gears
gears_EXTERNAL_OBJECTS =

external/glfw-3.1.2/examples/gears.exe: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj
external/glfw-3.1.2/examples/gears.exe: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/build.make
external/glfw-3.1.2/examples/gears.exe: external/glfw-3.1.2/src/libglfw3.a
external/glfw-3.1.2/examples/gears.exe: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/linklibs.rsp
external/glfw-3.1.2/examples/gears.exe: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/objects1.rsp
external/glfw-3.1.2/examples/gears.exe: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Work\autonomous-vehicles\opengl-env\win_build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable gears.exe"
	cd /d C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\gears.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/glfw-3.1.2/examples/CMakeFiles/gears.dir/build: external/glfw-3.1.2/examples/gears.exe

.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/build

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/requires: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.obj.requires

.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/requires

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/clean:
	cd /d C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples && $(CMAKE_COMMAND) -P CMakeFiles\gears.dir\cmake_clean.cmake
.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/clean

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Work\autonomous-vehicles\opengl-env C:\Work\autonomous-vehicles\opengl-env\external\glfw-3.1.2\examples C:\Work\autonomous-vehicles\opengl-env\win_build C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples C:\Work\autonomous-vehicles\opengl-env\win_build\external\glfw-3.1.2\examples\CMakeFiles\gears.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/depend

