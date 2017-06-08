# -*- coding: utf-8 -*-
#
#    Copied and Pasted by Jay (JongYoon) Kim, plenum@naver.com 
#
import numpy as np

class Config:
	def check(self):
		'''	정상적인 configuration이 되었는지 확인 하기 위해 '''
		if self.state_size <= 0:
			raise Exception("Invalid state size as an input to neural network")
		
		if self.action_num <= 0:
			raise Exception("Invalid state size as an input to neural network")
		
		return True

cfg = Config()

''' ======================================
	Common Constant Variables 
	======================================'''


# 입력 데이터를 이미지를 받을 경우 CNN으로 변경 
cfg.is_cnn = False

# input size 
'''
	AICar.cpp의 아래 코드의 값

	if(USE_CNN){
		WIDTH = 20;
		HEIGHT = 20;

		// add Speed, Direction, position as additional inputs
		NETWORK_INPUT_NUM = WIDTH*HEIGHT;
		INPUT_FRAME_CNT = 4;	
	}else
	{
		cfg.num_state = 36 * 1

		// add Speed, Direction, position as additional inputs
		NETWORK_INPUT_NUM = distances_from_sensors_.size();// + 2;	
		INPUT_FRAME_CNT = 1;
	}
'''
cfg.state_size = 36 * 1

# null state : episode가 종료되었을 경우 사용 
cfg.state_none = np.zeros(cfg.state_size)

# ouput size 
'''
	enum {
		ACT_ACCEL = 0,
		ACT_LEFT = 1,
		ACT_RIGHT = 2,
		ACT_BRAKE = 3,

		ACT_MAX = 4
	};
'''
cfg.action_num = 4 

# for learning rate 
cfg.learning_rate = 5e-3
cfg.lr_decay = .99

''' ======================================
	Constant Variables for A3C
	======================================'''

# neural network 업데이트를 위한 스레드 수 
cfg.optimizer_num = 2

# reward decay constants
cfg.gamma = 0.99
cfg.n_step_return = 8
cfg.gamma_n = cfg.gamma ** cfg.n_step_return

# for loss function 
cfg.loss_v = 0.5			# v loss coefficient
cfg.loss_entropy = 0.01 	# entropy coefficien

# mini_batch 
cfg.min_batch = cfg.n_step_return * 4

''' ======================================
	Constat Variables for Agent Class 
	======================================'''

# for epsilon
cfg.eps_start = 0.
cfg.eps_end = 0.
cfg.eps_steps = 75000

cfg.frame_max = 999999

''' ======================================
	Constant Variables for TCP Server
	======================================'''

cfg.port = 4655
cfg.recv_max_len = 1024


''' ======================================
	Constant Variables for Message Queue Server
	======================================'''

# ../opengl-env/main.cpp - CAR_PLAYED_NUM_MAX = 32
cfg.agent_max_num = 32

''' ======================================
	Miscellaneous Constat Variables
	======================================'''

cfg.float_size = 4
cfg.int_size = 4
cfg.backlog = 1

# end of file
