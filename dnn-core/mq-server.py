# -*- coding: utf-8 -*-
#
#    Copied and Pasted by Jay (JongYoon) Kim, plenum@naver.com 
#

#!/usr/bin/env python
''' zeromq server example.

    One front facing server and three workers are instantiated.
    Front facing server accepts connections from multiple clients and distributes computation requests among workers. 
    Also it collects computed results from workers and send the result back to the original client.

    Demonstrables:
        Multiple client connections.
        Work distribution among a pool of workers. '''

# Author - Kasun Herath <kasunh01 at gmail.com>
# Source - https://github.com/kasun/zeromq-client-server.git

import threading
import zmq
import struct

from config import cfg
from agent import Agent
from a3c import A3C, Optimizer

#----------
class MQ_Server(object):
    ''' Front facing server. '''

    def __init__(self):
        self.zmq_context = zmq.Context()

    def start(self):
        ''' Main execution. 
            Instantiate workers, Accept client connections, 
            distribute computation requests among workers and route computed results back to clients. '''

        self.dnn = A3C()
        self.optimizers = [Optimizer(self.dnn) for i in range(cfg.optimizer_num)]

        # Front facing socket to accept client connections.
        socket_front = self.zmq_context.socket(zmq.ROUTER)
        #socket_front.bind('tcp://127.0.0.1:5001')
        socket_front.bind('ipc:///tmp/deeplearning.zmq')

        # Backend socket to distribute work.
        socket_back = self.zmq_context.socket(zmq.DEALER)
        #socket_back = self.zmq_context.socket(zmq.REP)
        socket_back.bind('inproc://backend')

        workers = []

        # Start three workers.
        for i in range(0,cfg.agent_max_num - 1):
            worker = Worker(self.zmq_context, i, self.dnn)
            worker.start()
            workers.append(worker)

        self.run_optimizer()

        # Use built in queue device to distribute requests among workers.
        # What queue device does internally is,
        #   1. Read a client's socket ID and request.
        #   2. Send socket ID and request to a worker.
        #   3. Read a client's socket ID and result from a worker.
        #   4. Route result back to the client using socket ID. 
        try:
            zmq.device(zmq.QUEUE, socket_front, socket_back)
        except KeyboardInterrupt:
            print 'interrupted!'

        # process termination 
        self.stop_optimizer()

        for worker in workers:
            worker.stop()

        socket_front.close()
        socket_back.close()
        self.zmq_context.term()

        for worker in workers:
            worker.join()

        print "\nServer(Message Queue type) stopped!!!!\n"

    def run_optimizer(self):
        ''' Optimizers run '''
        for opts in self.optimizers:
            opts.start()

    def stop_optimizer(self):
        ''' Optimizers stopped '''
        for opts in self.optimizers:
            opts.stop()
        for opts in self.optimizers:
            opts.join()

class Worker(threading.Thread):
    ''' Workers accept computation requests from front facing server.
        Does computations and return results back to server. '''

    def __init__(self, zmq_context, _id, dnn):
        threading.Thread.__init__(self)
        self.zmq_context = zmq_context
        self.worker_id = _id

        self.agent = Agent(dnn)

        self.stop_signal = False

    def run(self):
        ''' Main execution. '''

        # Socket to communicate with front facing server.
        self.socket = self.zmq_context.socket(zmq.DEALER)
        #self.socket = self.zmq_context.socket(zmq.REP)
        self.socket.connect('inproc://backend')

        while not self.stop_signal:
            try:
                # First string recieved is socket ID of client
                #client_id = self.socket.recv()
                #recv = self.socket.recv()
                client_id, recv = self.socket.recv_multipart()
                pack_size = len(recv)
                #print pack_size

                if pack_size == cfg.int_size:
                    ID = struct.unpack("@i", recv)[0]
                    print "client_id(", client_id, ") - worker id(", self.worker_id, ")"
                elif pack_size == cfg.int_size + cfg.state_size * cfg.float_size:    # in case of getting action 
                    #print "get ACTION"
                    self.process_get_action(client_id, recv)
                else:
                    self.process_train(client_id, recv, pack_size)
    
            except Exception as err:
                break;

    def stop(self):
        ''' Each worker stopped. '''

        #print "Worker(", self.worker_id, ") is closed!"
        self.stop_signal = True
        self.socket.close()
        self.socket = None

    def process_get_action(self, client_id, packet):
        ''' Action from Neural Network '''

        fmt =  "@i" + str(cfg.state_size) + "f"
        #print "fmt:", fmt, " size:", struct.calcsize(fmt)
        raw = struct.unpack(fmt, packet)

        id_ = raw[0]
        #print "ID: ", id_
        state = raw[1:]
        #print state

        #a = agent.act(state)
        a = self.agent.act(state)
        #print "ID: ", id_, " Action: ", a
        a = struct.pack("@i",int(a))
        self.socket.send(client_id, zmq.SNDMORE)
        self.socket.send(a)

    def process_train(self, client_id, packet, pack_size):

        # packet size when an episode is terminated 
        packet_size_term = cfg.int_size + \
                            cfg.state_size * cfg.float_size +\
                            cfg.int_size +\
                            cfg.float_size

        # without next state
        if pack_size == packet_size_term :
            fmt =  "@i" + str(cfg.state_size) + "fif"
            raw = struct.unpack(fmt, packet)

            id_ = raw[0]
            s = raw[1:cfg.state_size+1]
            a = raw[cfg.state_size+1]
            r = raw[cfg.state_size+2]
            s_ = None
            self.agent.train(s, a, r, s_)

        # continuing 
        #continue_size = cfg.int_size + cfg.state_size * cfg.float_size * 2 + cfg.int_size + cfg.float_size
        else:
            fmt =  "@i" + str(cfg.state_size) + "fif" + str(cfg.state_size) + "f"
            #print "fmt:", fmt, " pack_size:", struct.calcsize(fmt)
            raw = struct.unpack(fmt, packet)
            
            id_ = raw[0]
            s = raw[1:cfg.state_size+1]
            a = raw[cfg.state_size+1]
            r = raw[cfg.state_size+2]
            s_ = raw[cfg.state_size+3:]
            self.agent.train(s, a, r, s_)
            '''
            print "ID: ", id_
            print "state"
            print s 
            print "Action:", a
            print "Reward:", r
            print "next state"
            print s_
            print "--------------"
            '''

if __name__ == '__main__':
    server = MQ_Server().start()
