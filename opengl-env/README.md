## Introduction  

This application is based on [Tutorial2: The first triangle](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/) in opengl-tutorial in terms of [source-code structure and build](https://github.com/opengl-tutorials/ogl), and the idea comes from [Dr. Jeong-Mo Hong](http://blog.naver.com/atelierjpro) in [Dongguk Univeristy](http://simulation.dongguk.edu/mediawiki/index.php/Main_Page).  
  
## Build Environment  

### Linux / MacOS  

My build environment on MacOS has been already done before this project, so you might need more than what I describe below if you fail to build. Maybe, c++11/14? :)  
  
**1. CMake**  
  
You can find an installation file from [CMake site](https://cmake.org/download/)  
  
**2. Boost**  
  
	// MacOS  
	brew install boost  
	
	// Linux  
	sudo apt-get install boost  
  
**3. ZeroMQ**  
  
	// MacOS  
	brew install zeromq  
	
	// Linux  
	sudo apt-get install zeromq  
  
    
### Windows  

Please make sure that the instruction below is **ONLY tested in Win7 64bits**.  

**1. MinGW + Boost**  

Instead of using visual studio as a compiler, I had to choose this, because I wanted to minimize effort to build on Windows, and I found really nice package with no installation from [nuwen.net](https://nuwen.net/mingw.html) where MingGW, boost and git are provided together with no installation. Just unzip and use it with the batch file named **"open__distro__window.bat"** to launch terminal to build this application.  

- Download and click **mingw-15.0.exe** from the site  
- Move the unzipped folder named "MinGW" to c:\  
- Make a shorcut of the batch file in desktop for your convenience  
- Type "c++ --version" on a terminal
  
  
**2. ZeroMQ**  

The best way to use ZeroMQ is to build it by yourself, but this application has header files and libraries in external folder for your convenience, so you have nothing to do if you do not face any problem.  
  
**3. CMake**  
  
You can find an installation file from [CMake site](https://cmake.org/download/)  

## Build  
  
### Linux / MacOS  
  
	mkdir build
	cd build
	cmake ..
	make  
	./autonomous_vehicle 10			<- upto 22 vehicles
  
### Windows  
  
You can easily build this with **win_build** directory that I made. However, you need to follow the instruction, **"Building on Windows"** in the [opengl tutorial site](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/), if you want to change.   
  
	cd win_build
	make 
	autonomous_vehicles.exe 10  
  
## Source Codes  
  
	# sub directories
	---- external 			: opengl wrappers, zeromq
	---- win_build			: build for Windows OS 
	
	# main files 
	- CMakeLists.txt		: cmake file  
	- main.cpp			: entry function 
	- AIVehicle.h			: AI Vehicle class
	- AIVehicle.cpp			: implementation of AI Vehicle
	- SelfDrivingWorld.h		: world where vehicles are driving
	- SelfDrivingWorld.cpp		: implementation of World
	- DNN_TCP.h			: DNN over TCP class
	- DNN_TCP.cpp			: implementation using boost/asio  
	- DNN_MQ.h			: DNN over Message Queue class
	- DNN_MQ.cpp			: implementation using ZeroMQ 

	# from https://github.com/jmhong-simulation/2016FallCSE2022/tree/master/Week13/CarDriving2D_Labversion  
	- Physics.h			: collision check class  
	- Object.h			: OpenGL Object class 
	- Object.cpp			: implementation of Object class 
	- LineObj.h			: line class inherited from Object class
	- SquareObj.h			: squre object class inherited from Object class
	- SquareObj.cpp			: implementation of square object  
	- Scene.h			: scene class for drawing world
	- Scene.cpp			: implementation of scene class 

	# from OpenGL Turorial  
	- shader.hpp		
	- shader.cpp		
	- SimpleFragmentShader.fragmentshader  
	- SimpleVertextShader.vertexshader   
	
	
  
