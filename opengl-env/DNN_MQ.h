#pragma once

#include "Common.h"
#include "zmq.h"

/**
 *   Deep Neural Network over Message Queue Class 
 */
class DNN_MQ {
public:
	DNN_MQ(const int& id);
	~DNN_MQ();

	void initialize();
	void close();

	label_t getAction(vec_t& state);
	void train(vec_t& s1, label_t& a, float& r, vec_t& s2);

protected:
	int id_;											// ai car id
	//  Socket to talk to clients
    void *context_;
    void *socket_;

};

// end of file
