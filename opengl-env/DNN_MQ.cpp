#include <iostream>
//#include "msgpack.hpp"	// msgpack을 이용하면 쉽게 프로토콜 정의할 수 있음. 여기서는 간단히 길이로... 

#include "AIVehicle.h"
#include "DNN_MQ.h"

DNN_MQ::DNN_MQ(const int& id)
{
	context_ = nullptr;
	socket_ = nullptr;

	id_ = id;
	int i = 0;
	while(id_) {
		id_ = id_ >> 1;
		++i;
	}
	id_ = i;
	//std::cout << "DNN_MQ constructed (ID:" << id_ << ")"<< endl;
}

DNN_MQ::~DNN_MQ(){
	close();
}

void DNN_MQ::initialize(){
	context_ = zmq_ctx_new ();
	socket_ = zmq_socket (context_, ZMQ_DEALER);
	
	char id = id_ +'0';
	zmq_setsockopt(socket_, ZMQ_IDENTITY, &id, sizeof(id));
	int rc = zmq_connect (socket_, "ipc:///tmp/deeplearning.zmq");
    assert (rc == 0);

    zmq_send (socket_, &id_, sizeof(int), 0);
    //assert (rc == sizeof(int));
}

void DNN_MQ::close(){
	if(socket_ != nullptr)	zmq_close(socket_);
	if(context_ != nullptr)	zmq_ctx_term(context_);
}

label_t DNN_MQ::getAction(vec_t& state){
	label_t action = AIVehicle::ACT_MAX;
	if(state.size() == 0) return action;
	//std::cout << "DNN_MQ::getAction - " << state.size() * sizeof(float) << endl;

	const int max_pack_size = 1024;
	char packet[max_pack_size];
	int total_pack_size = 0;
	int rc;

	// [1] id
	int pack_size = sizeof(int);
	memcpy (packet + total_pack_size, &id_, pack_size);
	total_pack_size += pack_size;

	// [2] current state
	pack_size = state.size() * sizeof(float);
	memcpy (packet + total_pack_size , reinterpret_cast<char*>(state.data()), pack_size);
	total_pack_size += pack_size;

	assert( total_pack_size < max_pack_size) ;
	rc = zmq_send (socket_,  packet, total_pack_size, 0);
	assert (rc == total_pack_size);

	//  Get the action
	pack_size = sizeof(label_t);
   	rc = zmq_recv (socket_, &action, pack_size, 0);
    assert (rc == pack_size);
	
	return action;
}

void DNN_MQ::train(vec_t& s1, label_t& a, float& r, vec_t& s2){
	if(s1.size() == 0) return;

	const int max_pack_size = 1024;
	char packet[max_pack_size];
	int total_pack_size = 0;
	int rc;

	// [1] id
	int pack_size = sizeof(int);
	memcpy (packet + total_pack_size, &id_, pack_size);
	total_pack_size += pack_size;

	//std::cout << "ID: " << id_ << endl;

	// [2] current state
	pack_size = s1.size() * sizeof(float);
	memcpy (packet + total_pack_size , reinterpret_cast<char*>(s1.data()), pack_size);
	total_pack_size += pack_size;

	//std::cout << "next state" << endl;
	//for(int i = 0 ; i < s1.size() ; ++i){
	//	std::cout << s1[i] << " ";
	//}
	//std::cout << endl;

	// [3] action 
	pack_size = sizeof(label_t);
	memcpy (packet + total_pack_size , &a, pack_size);
	total_pack_size += pack_size;
	//std::cout << "Action: " << a << endl;

	// [4] reward
	pack_size = sizeof(float);
	memcpy (packet + total_pack_size , &r, pack_size);
	total_pack_size += pack_size;
	//std::cout << "Reward: " << r << endl;

	if(s2.size()){
		// [5] next state
		pack_size = s2.size() * sizeof(float);
		memcpy (packet + total_pack_size , reinterpret_cast<char*>(s2.data()), pack_size);
		total_pack_size += pack_size;

		//std::cout << "next state" << endl;
		//for(int i = 0 ; i < s2.size() ; ++i){
		//	std::cout << s2[i] << " ";
		//}
		//std::cout << endl;
		//std::cout << "----------------"<< endl;
	}

	assert( total_pack_size < max_pack_size) ;
	rc = zmq_send (socket_,  packet, total_pack_size, 0);
	assert (rc == total_pack_size);
}

// end of file 
