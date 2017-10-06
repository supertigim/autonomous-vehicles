#pragma once 

#include "SquareObj.h"
#include "LineObj.h"
#include "AIVehicle.h"
#include "Scene.h"

class Agent;

class SelfDrivingWorld : public Scene
{
public:
	enum {
		OBJ_GR_ENV = 0,			// Object Group for Environments or map obstacles
		OBJ_GR_VEH = 1,			// Objext Group for vehicles
		OBJ_GR_MAX = 2
	};
	
private:
	glm::mat4 View_;
	GLuint MatrixID_;
	glm::mat4 Projection_;

	int input_frame_cnt_;
	int is_training_;

	std::vector<std::vector<std::unique_ptr<Object>>> obj_list_;
public:
	static SelfDrivingWorld& get(){
		static SelfDrivingWorld main_scene;
		return main_scene;
	}


	int is_training() {return is_training_;}

	void initialize(boost::asio::io_service& ios, int car_num);
	void createAIVehicles(const int& nums, boost::asio::io_service& ios);

	// flag = 0 : continue, 1 : terminal
	void update(const bool& update_render_data, float& reward, int& flag);

	const std::vector<std::vector<std::unique_ptr<Object>>>& getObjs() { return obj_list_;}//return obj_list;}
	const std::vector<std::unique_ptr<Object>>& getObjects() { return obj_list_[OBJ_GR_ENV];}//return obj_list;}
	const std::vector<std::unique_ptr<Object>>& getVehicles() { return obj_list_[OBJ_GR_VEH];}//return car_list_; }

	void render();
	void run();
	

	bool handleKeyInput();

//	label_t getActionFromCloud(vec_t& state);
	void trainCloud(vec_t& s1, label_t& a, float& r, vec_t& s2);

private:
	SelfDrivingWorld();

	void createScene_RandomObstacles();
	void createScene_Basic();
	void createScene_Road();
};

// end of file
