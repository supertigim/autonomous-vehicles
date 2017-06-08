# -*- coding: utf-8 -*-
#
#    Copied and Pasted by Jay (JongYoon) Kim, plenum@naver.com 
#
from socket import *
from select import select
import struct

from config import cfg
from agent import Agent
from a3c import A3C, Optimizer

#----------
# [Reference]
# http://wangx.in/post/multi-user_chatting_program_implemented_in_python/
#
class TCP_Server:
	def __init__(self):
		self.eps_start = cfg.eps_start
		self.eps_end = cfg.eps_end
		self.eps_steps = cfg.eps_steps

		self.port = cfg.port
		self.max_recv_len = cfg.recv_max_len
		self.server = None

		self.client_list = None
		self.input_event = []
		self.backlog = cfg.backlog

		self.agents = {}

		self.dnn = A3C()
		self.optimizers = [Optimizer(self.dnn) for i in range(cfg.optimizer_num)]

	def run(self):
		input_event = []
		# create tcp socket
		self.server = socket(AF_INET, SOCK_STREAM)
		self.server.bind(('',self.port))
		self.server.listen(self.backlog)
		self.input_event.append(self.server)

		self.run_optimizer()

		try:
			while(True):
				self.run_network()
		except KeyboardInterrupt:
			print 'interrupted!'

		self.stop_optimizer()

	def add_new_client(self,sock):
		new_sock, addr = sock.accept()
		self.input_event.append(new_sock)
		print "Connected", new_sock, " ", addr, " id: ", id		

	def remove_client(self,sock):
		print "Disconnected"
		#del self.agents[...] # To-do:
		sock.close()
		self.input_event.remove(sock)

	def create_agent(self,sock,packet):
		ID = struct.unpack("@i", packet)[0]
		#print "###### ID ", ID
		if not ID in self.agents:
			self.agents[ID] = Agent(self.dnn)

		ok = "ok"
		sock.send(ok)

	def process_get_action(self, sock, packet):
		fmt =  "@i" + str(cfg.state_size) + "f"
		#print "fmt:", fmt, " size:", struct.calcsize(fmt)
		raw = struct.unpack(fmt, packet)
		id_ = raw[0]
		state = raw[1:]

		#a = agent.act(state)
		a = self.agents[id_].act(state)
		#print "ID: ", id_, " Action: ", a
		a = struct.pack("@i",a)
		sock.send(a)

	def process_train(self, sock, packet, size):
		end_size = cfg.int_size + cfg.state_size * cfg.float_size + cfg.int_size + cfg.float_size

		# without next state
		if size == end_size :
			fmt =  "@i" + str(cfg.state_size) + "fif"
			raw = struct.unpack(fmt, packet)

			id_ = raw[0]
			s = raw[1:cfg.state_size+1]
			a = raw[cfg.state_size+1]
			r = raw[cfg.state_size+2]
			s_ = None
			self.agents[id_].train(s, a, r, s_)

		# continuing 
		#continue_size = cfg.int_size + cfg.state_size * cfg.float_size * 2 + cfg.int_size + cfg.float_size
		else:
			fmt =  "@i" + str(cfg.state_size) + "fif" + str(cfg.state_size) + "f"
			#print "fmt:", fmt, " size:", struct.calcsize(fmt)
			raw = struct.unpack(fmt, packet)
			
			id_ = raw[0]
			s = raw[1:cfg.state_size+1]
			a = raw[cfg.state_size+1]
			r = raw[cfg.state_size+2]
			s_ = raw[cfg.state_size+3:]
			self.agents[id_].train(s, a, r, s_)

		ok = "ok"
		sock.send(ok)

	def run_network(self):
		inputready,outputready,exceptready = select(self.input_event,[],[])

		for sock in inputready:
			if sock == self.server:
				self.add_new_client(sock)
			else:
				recv_data = sock.recv(cfg.recv_max_len)

				if len(recv_data) == 0:
					self.remove_client(sock)
					# break #self.on_close()
				else:
					size = len(recv_data)
					#print "Received - ", size

					if size == cfg.int_size:
						self.create_agent(sock,recv_data)
					elif size == cfg.int_size + cfg.state_size * cfg.float_size:	# in case of getting action 
						self.process_get_action(sock,recv_data)
					else:	# in case of training
						self.process_train(sock,recv_data,size)

	def run_optimizer(self):
		for opts in self.optimizers:
			opts.start()

	def stop_optimizer(self):
		for opts in self.optimizers:
			opts.stop()
		for opts in self.optimizers:
			opts.join()

if __name__ == "__main__":
	
	server = TCP_Server()
	server.run()

	print "\nServer(TCP type) stopped!!!\n"

# end of file
