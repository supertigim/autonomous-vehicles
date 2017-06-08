# -*- coding: utf-8 -*-
#
#    Copied and Pasted by Jay (JongYoon) Kim, plenum@naver.com 
#
import numpy as np
import tensorflow as tf

import time, threading

from keras.models import *
from keras.layers import *
from keras.constraints import maxnorm
from keras import backend as K
from keras.layers.advanced_activations import LeakyReLU

from config import cfg

#---------
class A3C:
	train_queue = [ [], [], [], [], [] ]	# s, a, r, s', s' terminal mask
	lock_queue = threading.Lock()

	def __init__(self):
		self.session = tf.Session()
		K.set_session(self.session)
		K.manual_variable_initialization(True)

		self.model = self._build_model()
		self.graph = self._build_graph(self.model)

		self.session.run(tf.global_variables_initializer())
		self.default_graph = tf.get_default_graph()

		self.default_graph.finalize()	# avoid modifications

	def _build_model(self):

		neurons = (cfg.state_size + cfg.action_num)
		l_input = Input( batch_shape=(None, cfg.state_size) )
		
		'''
		#l_dense = Dense(16, activation='relu')(l_input)
		l_dense1 = Dense(neurons, activation='linear')(l_input)
		l_dense1_1 = LeakyReLU(alpha=0.1)(l_dense1)
		l_dense2 = Dense(neurons, activation='linear')(l_dense1_1)
		l_dense2_1 = LeakyReLU(alpha=0.1)(l_dense2)
		l_dense3 = Dense(neurons, activation='linear')(l_dense2_1)
		l_dense3_1 = LeakyReLU(alpha=0.1)(l_dense3)
		l_dense4 = Dense(neurons, activation='linear')(l_dense3_1)
		l_dense4_1 = LeakyReLU(alpha=0.1)(l_dense3)
		#l_dense5 = Dense(neurons, activation='linear')(l_dense4_1)
		#l_dense5_1 = LeakyReLU(alpha=0.1)(l_dense5)
		#'''
		
		#'''
		l_dense1 = Dense(neurons, activation='relu', kernel_initializer='he_uniform',kernel_constraint=maxnorm(3))(l_input)
		#l_dense1_1 = Dropout(0.2)(l_dense1)
		l_dense4 = Dense(neurons, activation='relu', kernel_initializer='he_uniform', kernel_constraint=maxnorm(3))(l_dense1)
		#l_dense3 = Dense(neurons, activation='relu', kernel_initializer='he_uniform', kernel_constraint=maxnorm(3))(l_dense2)
		#l_dense4 = Dense(neurons, activation='relu', kernel_initializer='he_uniform', kernel_constraint=maxnorm(3))(l_dense3)
		#l_dense5 = Dense(neurons, activation='relu', kernel_initializer='he_uniform', kernel_constraint=maxnorm(3))(l_dense4)
		#l_dense6 = Dense(neurons, activation='relu', kernel_initializer='he_uniform', kernel_constraint=maxnorm(3))(l_dense5)
		#l_dense4 = Dense(neurons, activation='tanh', kernel_initializer='uniform')(l_dense3)
		#'''

		out_actions = Dense(cfg.action_num, activation='softmax')(l_dense4)
		out_value   = Dense(1, activation='linear')(l_dense4)

		model = Model(inputs=[l_input], outputs=[out_actions, out_value])
		model._make_predict_function()	# have to initialize before threading

		return model

	def _build_graph(self, model):
		s_t = tf.placeholder(tf.float32, shape=(None, cfg.state_size))
		a_t = tf.placeholder(tf.float32, shape=(None, cfg.action_num))
		r_t = tf.placeholder(tf.float32, shape=(None, 1)) # not immediate, but discounted n step reward
		
		p, v = model(s_t)

		log_prob = tf.log( tf.reduce_sum(p * a_t, axis=1, keep_dims=True) + 1e-10)
		advantage = r_t - v

		loss_policy = - log_prob * tf.stop_gradient(advantage)										# maximize policy
		loss_value  = cfg.loss_v * tf.square(advantage)												# minimize value error
		entropy = cfg.loss_entropy * tf.reduce_sum(p * tf.log(p + 1e-10), axis=1, keep_dims=True)	# maximize entropy (regularization)

		self.loss_total = tf.reduce_mean(loss_policy + loss_value + entropy)

		optimizer = tf.train.RMSPropOptimizer(cfg.learning_rate, decay=cfg.lr_decay)
		#optimizer = tf.train.AdamOptimizer(cfg.learning_rate)
		minimize = optimizer.minimize(self.loss_total)

		return s_t, a_t, r_t, minimize

	def optimize(self):
		if len(self.train_queue[0]) < cfg.min_batch:
			time.sleep(0)	# yield
			return

		with self.lock_queue:
			if len(self.train_queue[0]) < cfg.min_batch:	# more thread could have passed without lock
				return 										# we can't yield inside lock

			s, a, r, s_, s_mask = self.train_queue
			self.train_queue = [ [], [], [], [], [] ]

		s = np.vstack(s)
		a = np.vstack(a)
		r = np.vstack(r)
		s_ = np.vstack(s_)
		s_mask = np.vstack(s_mask)

		if len(s) > 5*cfg.min_batch: print("Optimizer alert! Minimizing batch of %d" % len(s))

		v = self.predict_v(s_)
		r = r + cfg.gamma_n * v * s_mask	# set v to 0 where s_ is terminal state
		
		s_t, a_t, r_t, minimize = self.graph
		self.session.run(minimize, feed_dict={s_t: s, a_t: a, r_t: r})

		#cost = self.session.run(self.loss_total,feed_dict={s_t: s, a_t: a, r_t: r})
		#print "Loss= ", cost

	def train_push(self, s, a, r, s_):
		with self.lock_queue:
			self.train_queue[0].append(s)
			self.train_queue[1].append(a)
			self.train_queue[2].append(r)

			if s_ is None:
				self.train_queue[3].append(cfg.state_none)
				self.train_queue[4].append(0.)
			else:	
				self.train_queue[3].append(s_)
				self.train_queue[4].append(1.)

	def predict(self, s):
		with self.default_graph.as_default():
			p, v = self.model.predict(s)
			return p, v

	def predict_p(self, s):
		with self.default_graph.as_default():
			p, v = self.model.predict(s)		
			return p

	def predict_v(self, s):
		with self.default_graph.as_default():
			p, v = self.model.predict(s)		
			return v


#---------
class Optimizer(threading.Thread):
	stop_signal = False

	def __init__(self, dnn):
		threading.Thread.__init__(self)
		self.dnn = dnn

	def run(self):
		try:
			while not self.stop_signal:
				self.dnn.optimize()
		except KeyboardInterrupt:
			print 'interrupted!'

	def stop(self):
		self.stop_signal = True
		print "Optimizer Stopped!!!"

# end of file
