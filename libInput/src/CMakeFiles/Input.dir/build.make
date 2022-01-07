# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/netpipe/Desktop/libInput

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/netpipe/Desktop/libInput

# Include any dependencies generated for this target.
include src/CMakeFiles/Input.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/Input.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/Input.dir/flags.make

src/CMakeFiles/Input.dir/InputController.cpp.o: src/CMakeFiles/Input.dir/flags.make
src/CMakeFiles/Input.dir/InputController.cpp.o: src/InputController.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/libInput/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/Input.dir/InputController.cpp.o"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Input.dir/InputController.cpp.o -c /home/netpipe/Desktop/libInput/src/InputController.cpp

src/CMakeFiles/Input.dir/InputController.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Input.dir/InputController.cpp.i"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/libInput/src/InputController.cpp > CMakeFiles/Input.dir/InputController.cpp.i

src/CMakeFiles/Input.dir/InputController.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Input.dir/InputController.cpp.s"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/libInput/src/InputController.cpp -o CMakeFiles/Input.dir/InputController.cpp.s

src/CMakeFiles/Input.dir/x11/X11InputController.cpp.o: src/CMakeFiles/Input.dir/flags.make
src/CMakeFiles/Input.dir/x11/X11InputController.cpp.o: src/x11/X11InputController.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/libInput/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/Input.dir/x11/X11InputController.cpp.o"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Input.dir/x11/X11InputController.cpp.o -c /home/netpipe/Desktop/libInput/src/x11/X11InputController.cpp

src/CMakeFiles/Input.dir/x11/X11InputController.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Input.dir/x11/X11InputController.cpp.i"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/libInput/src/x11/X11InputController.cpp > CMakeFiles/Input.dir/x11/X11InputController.cpp.i

src/CMakeFiles/Input.dir/x11/X11InputController.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Input.dir/x11/X11InputController.cpp.s"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/libInput/src/x11/X11InputController.cpp -o CMakeFiles/Input.dir/x11/X11InputController.cpp.s

src/CMakeFiles/Input.dir/x11/X11InputDevice.cpp.o: src/CMakeFiles/Input.dir/flags.make
src/CMakeFiles/Input.dir/x11/X11InputDevice.cpp.o: src/x11/X11InputDevice.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/libInput/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/Input.dir/x11/X11InputDevice.cpp.o"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Input.dir/x11/X11InputDevice.cpp.o -c /home/netpipe/Desktop/libInput/src/x11/X11InputDevice.cpp

src/CMakeFiles/Input.dir/x11/X11InputDevice.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Input.dir/x11/X11InputDevice.cpp.i"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/libInput/src/x11/X11InputDevice.cpp > CMakeFiles/Input.dir/x11/X11InputDevice.cpp.i

src/CMakeFiles/Input.dir/x11/X11InputDevice.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Input.dir/x11/X11InputDevice.cpp.s"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/libInput/src/x11/X11InputDevice.cpp -o CMakeFiles/Input.dir/x11/X11InputDevice.cpp.s

src/CMakeFiles/Input.dir/x11/X11Keyboard.cpp.o: src/CMakeFiles/Input.dir/flags.make
src/CMakeFiles/Input.dir/x11/X11Keyboard.cpp.o: src/x11/X11Keyboard.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/libInput/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/Input.dir/x11/X11Keyboard.cpp.o"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Input.dir/x11/X11Keyboard.cpp.o -c /home/netpipe/Desktop/libInput/src/x11/X11Keyboard.cpp

src/CMakeFiles/Input.dir/x11/X11Keyboard.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Input.dir/x11/X11Keyboard.cpp.i"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/libInput/src/x11/X11Keyboard.cpp > CMakeFiles/Input.dir/x11/X11Keyboard.cpp.i

src/CMakeFiles/Input.dir/x11/X11Keyboard.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Input.dir/x11/X11Keyboard.cpp.s"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/libInput/src/x11/X11Keyboard.cpp -o CMakeFiles/Input.dir/x11/X11Keyboard.cpp.s

src/CMakeFiles/Input.dir/x11/X11Mouse.cpp.o: src/CMakeFiles/Input.dir/flags.make
src/CMakeFiles/Input.dir/x11/X11Mouse.cpp.o: src/x11/X11Mouse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/libInput/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/Input.dir/x11/X11Mouse.cpp.o"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Input.dir/x11/X11Mouse.cpp.o -c /home/netpipe/Desktop/libInput/src/x11/X11Mouse.cpp

src/CMakeFiles/Input.dir/x11/X11Mouse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Input.dir/x11/X11Mouse.cpp.i"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/libInput/src/x11/X11Mouse.cpp > CMakeFiles/Input.dir/x11/X11Mouse.cpp.i

src/CMakeFiles/Input.dir/x11/X11Mouse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Input.dir/x11/X11Mouse.cpp.s"
	cd /home/netpipe/Desktop/libInput/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/libInput/src/x11/X11Mouse.cpp -o CMakeFiles/Input.dir/x11/X11Mouse.cpp.s

# Object files for target Input
Input_OBJECTS = \
"CMakeFiles/Input.dir/InputController.cpp.o" \
"CMakeFiles/Input.dir/x11/X11InputController.cpp.o" \
"CMakeFiles/Input.dir/x11/X11InputDevice.cpp.o" \
"CMakeFiles/Input.dir/x11/X11Keyboard.cpp.o" \
"CMakeFiles/Input.dir/x11/X11Mouse.cpp.o"

# External object files for target Input
Input_EXTERNAL_OBJECTS =

src/libInput.a: src/CMakeFiles/Input.dir/InputController.cpp.o
src/libInput.a: src/CMakeFiles/Input.dir/x11/X11InputController.cpp.o
src/libInput.a: src/CMakeFiles/Input.dir/x11/X11InputDevice.cpp.o
src/libInput.a: src/CMakeFiles/Input.dir/x11/X11Keyboard.cpp.o
src/libInput.a: src/CMakeFiles/Input.dir/x11/X11Mouse.cpp.o
src/libInput.a: src/CMakeFiles/Input.dir/build.make
src/libInput.a: src/CMakeFiles/Input.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/netpipe/Desktop/libInput/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library libInput.a"
	cd /home/netpipe/Desktop/libInput/src && $(CMAKE_COMMAND) -P CMakeFiles/Input.dir/cmake_clean_target.cmake
	cd /home/netpipe/Desktop/libInput/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Input.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/Input.dir/build: src/libInput.a

.PHONY : src/CMakeFiles/Input.dir/build

src/CMakeFiles/Input.dir/clean:
	cd /home/netpipe/Desktop/libInput/src && $(CMAKE_COMMAND) -P CMakeFiles/Input.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/Input.dir/clean

src/CMakeFiles/Input.dir/depend:
	cd /home/netpipe/Desktop/libInput && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/netpipe/Desktop/libInput /home/netpipe/Desktop/libInput/src /home/netpipe/Desktop/libInput /home/netpipe/Desktop/libInput/src /home/netpipe/Desktop/libInput/src/CMakeFiles/Input.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/Input.dir/depend

