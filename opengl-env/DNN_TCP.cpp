#include <iostream>
#include "DNN_TCP.h"
#include "AIVehicle.h"

DNN_TCP::DNN_TCP(const int& id, boost::asio::io_service &ios)
	:id_(id)
{
	socket_ = std::make_shared<boost::asio::ip::tcp::socket>(ios);
}

DNN_TCP::~DNN_TCP(){
	close();
}

void DNN_TCP::initialize(){

	int port = 4655;
	std::string host = "127.0.0.1";
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);

	boost::system::error_code ec;
	socket_->connect(endpoint, ec);

	if (ec)	{
		std::cout << "Socket Connection Error" << endl;
		exit(0);
	}
	else{
		boost::asio::write(*socket_, boost::asio::buffer(&id_, sizeof(int)));
		
		char res[2]; // dummy for sync 
		boost::asio::read(*socket_, boost::asio::buffer(&res, sizeof(res)));
	}
}

void DNN_TCP::close(){
	if(socket_)
		socket_->close();
}

// [Reference]
// http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/http/client/sync_client.cpp
label_t DNN_TCP::getAction(vec_t& state){
	label_t action = AIVehicle::ACT_MAX;

	if(state.size() == 0) return action;

	std::vector<boost::asio::const_buffer> bufs;
	bufs.push_back(boost::asio::buffer(&id_, sizeof(int)));
	bufs.push_back(boost::asio::buffer(state, state.size()*sizeof(float_t)));

	boost::asio::write(*socket_, bufs);
	
	boost::asio::read(*socket_, boost::asio::buffer(&action, sizeof(label_t)));

	return action;
}

void DNN_TCP::train(vec_t& s1, label_t& a, float& r, vec_t& s2){
	if(s1.size() == 0) return;

	// http://think-async.com/Asio/boost_asio_1_3_1/doc/html/boost_asio/reference/buffer.html
	std::vector<boost::asio::const_buffer> bufs;
	bufs.push_back(boost::asio::buffer(&id_, sizeof(int)));
	bufs.push_back(boost::asio::buffer(s1, s1.size()*sizeof(float_t)));
	bufs.push_back(boost::asio::buffer(&a, sizeof(label_t)));
	bufs.push_back(boost::asio::buffer(&r, sizeof(float)));
	if(s2.size())
		bufs.push_back(boost::asio::buffer(s2, s2.size()*sizeof(float_t)));
	
	boost::asio::write(*socket_, bufs);

	char res[2]; // dummy for sync 
	boost::asio::read(*socket_, boost::asio::buffer(&res, sizeof(res)));
}
