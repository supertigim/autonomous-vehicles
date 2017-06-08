# CMake entry point
cmake_minimum_required (VERSION 2.6)
project( AUTONOMOUS_VEHICLES )

find_package(OpenGL REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies 
add_subdirectory (external)

MESSAGE( STATUS "Compiling Autonomous Vehicles" )

include_directories(
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
#	external/msgpack-c/include/
	.
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
set (EXTRA_BOOST_LIBS 
	"boost_filesystem"
	"boost_system"
	"boost_thread"
	"ws2_32"
)

file(
	COPY ${CMAKE_SOURCE_DIR}/external/zeromq/libzmq.dll
	DESTINATION ${CMAKE_BINARY_DIR}
)

include_directories(
	external/zeromq/include/
)

link_directories(
	${CMAKE_SOURCE_DIR}/external/zeromq
)

set (
	ZEROMQ_LIB 
	"zmq.dll"
)

else (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
set (EXTRA_BOOST_LIBS 
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
)

find_package(ZeroMQ REQUIRED)
include_directories(${ZeroMQ_INCLUDE_DIR})
set (
	ZEROMQ_LIB 
	${ZeroMQ_STATIC_LIBRARY}
)

endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
	${EXTRA_BOOST_LIBS}
	${ZEROMQ_LIB}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

# Application
add_executable(autonomous_vehicles 
	shader.cpp 
	Object.cpp 
	SquareObj.cpp 
	Scene.cpp

	AIVehicle.cpp
	SelfDrivingWorld.cpp
	Main.cpp

	DNN_TCP.cpp
	DNN_MQ.cpp
	
	SimpleFragmentShader.fragmentshader
	SimpleVertexShader.vertexshader
)

target_link_libraries(autonomous_vehicles
	${ALL_LIBS} 
)
