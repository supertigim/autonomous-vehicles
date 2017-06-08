## Introduction  

This is the project to see if **Reinforcement Learning** can be used in Drone management system.   
  
**[Video](https://www.youtube.com/watch?v=pdkdsPV1m_I)** : There are 22 vehicles driving on the world. Although the application runs so slowly, it definitely shows that they can make the rule to avoid collision with each other after only less 5 minutes training. :)  

## How to use  
  
1. Run **mq-server.py** or tcp-server.py in dnn-core directory first
2. Run **autonomous_vehicle** in opengl-env directroy after build
3. Push **"space bar"** if you want to train one vehicle, otherwise push **"a" key** to train **all**.  
4. Push "a" or "space" to stop trainig mode again 

## To-do List  

1. Save and load a trained model   
2. Convolutional Neural Network   
3. Unity3D based environment(application)   
4. Multi-Drone management system with [dronekit](http://dronekit.io/) and [Qgroundcontrol](http://qgroundcontrol.com/) 
    
## Open source list  
  
1. [OpenGL](https://www.opengl.org/) and Wrapper tools   
2. [boost/asio](http://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio.html)  
3. [ZeroMQ](http://zeromq.org/)  
4. [Anaconda](https://www.continuum.io/downloads)
5. [Tensorflow](https://www.tensorflow.org)  
6. [Keras](https://keras.io/)  
7. [CMake](https://cmake.org/)  
8. [MinGW](http://www.mingw.org/) ([nuwen.net](https://nuwen.net/))  
9. [Git](https://git-scm.com/)  
  
## Reference  

1. [Self Driving Car Application by Dr.Hong in Dongguk University](https://github.com/jmhong-simulation/2016FallCSE2022/tree/master/Week13/CarDriving2D_Labversion)
2. [OpenGL Tutorial](http://www.opengl-tutorial.org/)
3. [A3C Theory & Implementation](https://jaromiru.com/2017/02/16/lets-make-an-a3c-theory/) --> [Source Codes](https://github.com/jaara/AI-blog/blob/master/CartPole-A3C.py)  

## License  

**MIT**

