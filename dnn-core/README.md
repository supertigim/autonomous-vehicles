## Introduction  

Deep learning classes in python. Before you try, I strongly recommend to install and use Anaconda first.   

Among many reinforcement learning algorithm, I picked up Asynchronous advantage actor, A3C for short, because it outperforms others like DQN series in terms of training speed. You can simply test others with minor changes if you want. :)

To communicate with an environment, I choose ZeroMQ which supports various protocols such as tcp/ip and ipc (inter-process communication). mq-server.py is set to use **IPC as default** which means server and client run on a same computer.      
    
  
## Dependencies  
  
- python 2.7  
- numpy  
- tensorflow  
- keras  
- zeromq  
  
## How to use  
  
**Message Queue Service**  
  
	python mq-server.py  
  
**TCP only Service**  
  
	python tcp-server.py  
  
## Source Codes  
  
	- config.py		: hyper parameters   
	- agent.py		: agent class  
	- a3c.py		: [A3C algorithm class](https://github.com/jaara/AI-blog/blob/master/CartPole-A3C.py) 
	- mq-server.py	: server over Message Queue
	- tcp-server.py	: server over TCP
	