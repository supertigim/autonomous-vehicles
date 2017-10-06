#include <iostream>
#include "SelfDrivingWorld.h"
#include <glm/gtc/matrix_transform.hpp>

SelfDrivingWorld::SelfDrivingWorld()
	:is_training_(0)
{}

void SelfDrivingWorld::initialize(boost::asio::io_service& ios, int car_num) {
	
	Scene::init();

	obj_list_.resize(OBJ_GR_MAX);

	// Create number of AI cars 
	createAIVehicles(car_num, ios);

	
	// Create World
	// 아래 셋 중에 하나로 환경을 만들 수 있음 
	//createScene_RandomObstacles();	// 다이아몬드 장애물을 랜덤으로 생성
	createScene_Road();					// 도로 모양 
	//createScene_Basic();
	

	// Camera matrix
	View_ = glm::lookAt(
		glm::vec3(0.5, 0.5, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0.5, 0.5, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  	// Head is up (set to 0,-1,0 to look upside-down)
	);

	MatrixID_ = glGetUniformLocation(programID, "MVP");
	Projection_ = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,100.0f); // In world coordinates
}

void SelfDrivingWorld::createAIVehicles(const int& nums, boost::asio::io_service& ios){

	for(int i = 0; i < nums; ++i){
		//std::cout << "Car num: "<< (1 << i) << endl;
		AIVehicle *car = new AIVehicle(1 << i, ios);		
		car->initialize();
		obj_list_[OBJ_GR_VEH].push_back(std::move(std::unique_ptr<AIVehicle>(car)));
	}
}

void SelfDrivingWorld::createScene_Road(){
	const float	world_center_x = 0.5f,
			world_center_y = 0.5,
			world_radius = 1.2f;
	
	// outer barrier
	{
		SquareObj* box = new SquareObj();
		box->initPos(glm::vec3(world_center_x, world_center_y, 0.0f), world_radius, world_radius);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(box)));
	}

	{
		SquareObj* box = new SquareObj();
		box->initPos(glm::vec3(world_center_x - 0.47f, world_center_y + 0.50f, 0.0f), 0.3f, 0.3f);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(box)));
	}

	{
		SquareObj* box = new SquareObj();
		box->initPos(glm::vec3(world_center_x - 0.47f, world_center_y - 0.50f, 0.0f), 0.3f, 0.3f);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(box)));
	}

	{
		SquareObj* box = new SquareObj();
		box->initPos(glm::vec3(world_center_x + 0.47f, world_center_y + 0.50f, 0.0f), 0.3f, 0.3f);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(box)));
	}

	{
		SquareObj* box = new SquareObj();
		box->initPos(glm::vec3(world_center_x + 0.47f, world_center_y - 0.50f, 0.0f), 0.3f, 0.3f);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(box)));
	}
}

void SelfDrivingWorld::createScene_Basic(){
	const float	world_center_x = 0.5f,
			world_center_y = 0.5,
			world_radius = 1.2f;

	// outer barrier		
	{
		Object *temp = new Object;
		temp->initCircle(glm::vec3(world_center_x, world_center_y, 0.0f), world_radius, 30);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}

	// inner barrier
	{
		Object *temp = new Object;
		temp->initCircle(glm::vec3(0.5f, 0.5f, 0.0f), 0.75f, 20);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}

	// hurdles for training 
	{
		Object *temp = new Object;
		//temp->initCircle(glm::vec3(0.98f, 0.0f, 0.0f), 0.05f, 6);
		temp->initCircle(glm::vec3(-0.43, 0.0f, 0.0f), 0.05f, 6);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}
	
	{
		Object *temp = new Object;
		temp->initCircle(glm::vec3(1.65f, 0.5f, 0.0f), 0.05f, 6);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}

	{
		Object *temp = new Object;
		//temp->initCircle(glm::vec3(0.98f, 1.0f, 0.0f), 0.05f, 6);
		temp->initCircle(glm::vec3(1.0f, 1.1f, 0.0f), 0.05f, 6);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}
	
	{
		Object *temp = new Object;
		//temp->initCircle(glm::vec3(0.5f, 1.45f, 0.0f), 0.05f, 6);
		temp->initCircle(glm::vec3(0.2f, 1.4f, 0.0f), 0.05f, 6);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}

	{
		Object *temp = new Object;
		//temp->initCircle(glm::vec3(0.5f, 1.45f, 0.0f), 0.05f, 6);
		temp->initCircle(glm::vec3(0.990f, -0.35f, 0.0f), 0.05f, 6);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp)));
	}
}

void SelfDrivingWorld::createScene_RandomObstacles(){
	const float	world_center_x = 0.5f,
			world_center_y = 0.5,
			world_radius = 1.2f,
			obstacle_radius = 0.05f,
			margin = 0.05f;
	
	// outer barrier
	{
		SquareObj* box = new SquareObj();
		box->initPos(glm::vec3(world_center_x, world_center_y, 0.0f), world_radius, world_radius);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(box)));

	}

	for(int i = 0 ; i < 20; ++i){
		float x,y;
		while (true){
			x = uniform_rand(world_center_x - (world_radius - obstacle_radius - margin)
								, world_center_x + (world_radius - obstacle_radius - margin));
			y = uniform_rand(world_center_y - (world_radius - obstacle_radius - margin)
								, world_center_y + (world_radius - obstacle_radius - margin));

			if( world_radius > std::sqrt((x-world_center_x)*(x-world_center_x)
										 + (y-world_center_y)*(y-world_center_y)))
				break;
		}

		Object *temp = new Object;
		temp->initCircle(glm::vec3(x, y, 0.0f), obstacle_radius, 6);
		obj_list_[OBJ_GR_ENV].push_back(std::move(std::unique_ptr<Object>(temp))); 
	}
}

void SelfDrivingWorld::render(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);
	glEnableVertexAttribArray(0);

	for(int i = 0 ; i < obj_list_.size() ; ++i){
		for(int j = 0; j < obj_list_[i].size(); ++j){
			obj_list_[i][j]->render(MatrixID_, Projection_ * View_);
		}
	}

	glDisableVertexAttribArray(0);
	swapBuffers();
}

bool SelfDrivingWorld::handleKeyInput(){
	glfwPollEvents();

	//if (getKeyPressed(GLFW_KEY_LEFT) == true) car_list_[0]->processInput(AIVehicle::ACT_LEFT);
	//else if (getKeyPressed(GLFW_KEY_RIGHT) == true) car_list_[0]->processInput(AIVehicle::ACT_RIGHT);

	//if (getKeyPressed(GLFW_KEY_UP) == true) car_list_[0]->processInput(AIVehicle::ACT_ACCEL);
	//else if (getKeyPressed(GLFW_KEY_DOWN) == true) car_list_[0]->processInput(AIVehicle::ACT_BRAKE);

	//if (getKeyPressed(GLFW_KEY_A) == true) car_list_[1]->processInput(AIVehicle::ACT_ACCEL);
	//else if (getKeyPressed(GLFW_KEY_Z) == true) car_list_[1]->processInput(AIVehicle::ACT_BRAKE);

	// Check if the ESC key was pressed or the window was closed
	if (getKeyPressed(GLFW_KEY_ESCAPE) || getWindowShouldClose())
		return false;

	if (getKeyPressed(GLFW_KEY_Q) == true) {
		//dqn_->getNetwork().save("SELF-DRIVING-VEHICLES-MODEL");
		std::cout << "writing complete" << endl;
	}

	static bool key_reset_flag = true;		// temporal variable
	if (getKeyPressed(GLFW_KEY_SPACE) == true || 
		getKeyPressed(GLFW_KEY_A) == true) {

		if(key_reset_flag == true) {
			if(!is_training_){
				if(getKeyPressed(GLFW_KEY_A) == true)	is_training_ = INT_MAX;
				else									is_training_ = 1 << uniform_rand(0, (int)obj_list_[OBJ_GR_VEH].size()-1);
				std::cout << "Back ground training mode" << endl;
			}
			else {
				is_training_ = 0;
				std::cout << "Interactive rendering mode" << endl;
			} 
			key_reset_flag = false;
		}
	}
	else {
		key_reset_flag = true;
	}

	return true;
}

void SelfDrivingWorld::run() {

	while(true)
	{
		if(!handleKeyInput()) break;
		
		for(int i = 0 ; i < obj_list_[OBJ_GR_VEH].size() ; ++i){
			obj_list_[OBJ_GR_VEH][i]->update();
		}
		
		render();
	}
	clear();
}

// end of file
