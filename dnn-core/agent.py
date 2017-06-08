# -*- coding: utf-8 -*-
#
#    Copied and Pasted by Jay (JongYoon) Kim, plenum@naver.com 
#
import numpy as np
import random
from config import cfg

#---------
class Agent:
	def __init__(self, brain):
		self.eps_start = cfg.eps_start
		self.eps_end   = cfg.eps_end
		self.eps_steps = cfg.eps_steps

		self.memory = []	# used for n_step return

		# Don't use R, because environment already calculates reward depending on how long episode is 
		self.R = 0.

		self.frames = 0
		self.brain = brain

	def getEpsilon(self):
		if(self.frames >= self.eps_steps):
			return self.eps_end
		else:
			return self.eps_start + self.frames * (self.eps_end - self.eps_start) / self.eps_steps	# linearly interpolate

	def act(self, s):
		eps = self.getEpsilon()	
		if self.frames < cfg.frame_max:		
			self.frames += 1

		if random.random() < eps:
			return random.randint(0, cfg.action_num - 1)

		else:
			s = np.array([s])
			p = self.brain.predict_p(s)[0]

			# a = np.argmax(p)
			a = np.random.choice(cfg.action_num, p=p)

			return a

	def train_(self, s, a, r, s_):
		def get_sample(memory, n):
			s, a, _, _  = memory[0]
			_, _, _, s_ = memory[n-1]

			return s, a, self.R, s_

		a_cats = np.zeros(cfg.action_num)	# turn action into one-hot representation
		a_cats[a] = 1 

		self.memory.append( (s, a_cats, r, s_) )

		self.R = ( self.R + r * cfg.gamma_n ) / cfg.gamma

		if s_ is None:
			while len(self.memory) > 0:
				n = len(self.memory)
				s, a, r, s_ = get_sample(self.memory, n)
				self.brain.train_push(s, a, r, s_)

				self.R = ( self.R - self.memory[0][2] ) / cfg.gamma 
				self.memory.pop(0)		

			self.R = 0

		if len(self.memory) >= cfg.n_step_return:
			s, a, r, s_ = get_sample(self.memory, cfg.n_step_return)
			self.brain.train_push(s, a, r, s_)

			self.R = self.R - self.memory[0][2]
			self.memory.pop(0)			
	
	def train(self, s, a, r, s_):
		def get_sample(memory, n):
			s, a, r, _  = memory[0]
			_, _, _, s_ = memory[n-1]

			return s, a, r, s

		a_cats = np.zeros(cfg.action_num)	# turn action into one-hot representation
		a_cats[a] = 1 

		self.memory.append( (s, a_cats, r, s_) )

		#self.R = ( self.R + r * cfg.gamma_n ) / cfg.gamma

		if s_ is None:
			while len(self.memory) > 0:
				n = len(self.memory)
				s, a, r, s_ = get_sample(self.memory, n)
				#s, a, r, s_ = self.memory[0]
				self.brain.train_push(s, a, r, s_)

				#self.R = ( self.R - self.memory[0][2] ) / cfg.gamma
				self.memory.pop(0)		

			#self.R = 0

		if len(self.memory) >= cfg.n_step_return:
			while len(self.memory) > 0:
				#s, a, r, s_ = get_sample(self.memory, cfg.n_step_return)
				s, a, r, s_ = self.memory[0]
				self.brain.train_push(s, a, r, s_)

				#self.R = self.R - self.memory[0][2]
				self.memory.pop(0)	
	
	# possible edge case - if an episode ends in <N steps, the computation is incorrect


# end of file
