#pragma once 

#include "Common.h"
#include <boost/asio.hpp>

/**
 *   Deep Neural Network over TCP Class 
 *
 *	 Port: 4655
 *   IP: 127.0.0.1 
 */
class DNN_TCP {
public:
	DNN_TCP(const int& id, boost::asio::io_service &ios);
	~DNN_TCP();
	void initialize();
	void close();

	label_t getAction(vec_t& state);
	void train(vec_t& s1, label_t& a, float& r, vec_t& s2);

protected:
	std::shared_ptr<boost::asio::ip::tcp::socket> socket_;	// tcp socket
	const int id_;											// ai car id
};

// End of file
