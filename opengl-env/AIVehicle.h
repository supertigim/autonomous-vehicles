#pragma once 

#include "SquareObj.h"
#include "LineObj.h"
#include "DNN_TCP.h"
#include "DNN_MQ.h"

/**
 *   AIVehicle class
 *
 *	 inherited from SqureObj class
 */
class AIVehicle : public SquareObj{
public:
	enum {
		ACT_ACCEL = 0,
		ACT_LEFT = 1,
		ACT_RIGHT = 2,
		ACT_BRAKE = 3,
		ACT_MAX = 4
	};

protected:
	const int ID_;
	LineObj sensing_lines;
	
	glm::vec3 dir_, vel_;
	glm::vec3 previous_pos_;

	float turn_coeff_;
	float accel_coeff_;
	float brake_coeff_;
	float fric;
	float sensing_radius;

	int sensor_min, sensor_max, sensor_di;
	vec_t distances_from_sensors_;
	
	float car_length_;
	float direction_degree_;

	// for CNN 
	std::deque<vec_t> past_states_;
	bool use_cnn_;

	// boost 라이브러리로 tcp통신
	//std::shared_ptr<DNN_TCP> dnn_;
	std::shared_ptr<DNN_MQ> dnn_;
	int init_sucess_;

	float NETWORK_INPUT_NUM;
	float INPUT_FRAME_CNT;
	int  WIDTH, HEIGHT;
	bool USE_CNN;
	
public:
	AIVehicle(const int& id, boost::asio::io_service& ios);
	void initialize();
	const int ID() {return ID_;}

	void setDirection(const float& dir);
	float getDirection() { return direction_degree_;}
	float getSpeed();

	//void drive();
	void update();
	void render(const GLint& MatrixID, const glm::mat4 vp);

	void processInput(const int& action);

protected:
	bool getState(vec_t& t);
	void turnLeft();
	void turnRight();
	void accel(float coeff = 0.0001f);
	void decel();
	
	void updateAll(label_t& action, float& reward, vec_t& t);
	void updateSensor();
	void updateStateVector();

	bool isTerminated();
	void calculateReward(float& reward);
	
	void makeImage(vec_t& t);
	bool validatePos(const float& x, const float& y);
};

// end of file
