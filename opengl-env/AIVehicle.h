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
		//ACT_STAY = 4,
		ACT_MAX = 4
	};

	static float NETWORK_INPUT_NUM;
	static float INPUT_FRAME_CNT;
	static int  WIDTH, HEIGHT;
	static bool USE_CNN;

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
	std::deque<std::unique_ptr<Object>> passed_pos_obj_list_;
	glm::vec3 passed_pos_; 
	float car_length_;
	float direction_degree_;

	// for CNN 
	std::deque<vec_t> past_states_;
	bool use_cnn_;

	// boost 라이브러리로 tcp통신
	//std::shared_ptr<DNN_TCP> dnn_;
	std::shared_ptr<DNN_MQ> dnn_;
	int init_sucess_;
	
public:
	AIVehicle(int id, boost::asio::io_service& ios);
	void initialize();
	int ID() {return ID_;}

	void setDirection(float dir);
	float getDirection() { return direction_degree_;}
	float getSpeed();

	void drive();
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
	void createSkidMark(const int& nums);
	void calculateReward(float& reward);
	
	void makeImage(vec_t& t);
	bool validatePos(float x, float y);
};

// end of file
