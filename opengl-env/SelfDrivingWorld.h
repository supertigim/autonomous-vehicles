#pragma once 

#include "SquareObj.h"
#include "LineObj.h"
#include "AIVehicle.h"
#include "Scene.h"

class Agent;

class SelfDrivingWorld : public Scene
{
public:
	std::vector<std::unique_ptr<AIVehicle>> car_list_;
	std::vector<std::unique_ptr<Object>> obj_list;
	
protected:
	glm::mat4 View_;
	GLuint MatrixID_;
	glm::mat4 Projection_;

	int input_frame_cnt_;
	int is_training_;

public:
	int is_training() {return is_training_;}

	void initialize(boost::asio::io_service& ios, int car_num);
	void createAIVehicles(int nums, boost::asio::io_service& ios);

	// flag = 0 : continue, 1 : terminal
	void update(const bool& update_render_data, float& reward, int& flag);

	const std::vector<std::unique_ptr<Object>>& getObjects() { return obj_list;}
	const std::vector<std::unique_ptr<AIVehicle>>& getCars() { return car_list_; }

	void render();
	void run();

	static SelfDrivingWorld& get(){
		static SelfDrivingWorld main_scene;
		return main_scene;
	}

	bool handleKeyInput();

	label_t getActionFromCloud(vec_t& state);
	void trainCloud(vec_t& s1, label_t& a, float& r, vec_t& s2);

protected:
	SelfDrivingWorld();

	void createScene_RandomObstacles();
	void createScene_Basic();
	void createScene_Road();
};

// end of file
