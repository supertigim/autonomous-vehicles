#include <memory>
#include <math.h>
#include <iostream>
#include <glm/gtx/transform.hpp>

#include "AIVehicle.h"
#include "SelfDrivingWorld.h"

const int INIT_TRY_CNT = 10;

AIVehicle::AIVehicle(const int& id, boost::asio::io_service& ios) 
		: ID_(id), car_length_(0.03f), fric(0.01f), turn_coeff_(2.0f), accel_coeff_(0.0001f), brake_coeff_(0.0003f)
		, sensing_radius(0.35), sensor_min(-170), sensor_max(180), sensor_di(10)
{
	for (int i = sensor_min; i <= sensor_max; i += sensor_di) {
		distances_from_sensors_.push_back(i);
	}
	
	USE_CNN = false;
	init_sucess_ = INIT_TRY_CNT;

	if(USE_CNN){
		WIDTH = 20;
		HEIGHT = 20;

		// add Speed, Direction, position as additional inputs
		NETWORK_INPUT_NUM = WIDTH*HEIGHT;
		INPUT_FRAME_CNT = 4;	
	}else
	{
		/*
		*	[같이 수정 1]
		*/
		// add Speed, Direction, position as additional inputs
		NETWORK_INPUT_NUM = distances_from_sensors_.size();// + 2;	
		//NETWORK_INPUT_NUM = distances_from_sensors_.size() + 2;	// 센서 외 추가 데이터 
		INPUT_FRAME_CNT = 1;
	}

	/*
	*	TCP 연결로 변경하려면 여기.. (물론 헤더 파일도~)	
	*/
	//dnn_ = std::make_shared<DNN_TCP>(ID_, ios);
	dnn_ = std::make_shared<DNN_MQ>(ID_);
	dnn_->initialize();

	//std::cout << "AI Car initialized" << endl;
}

void AIVehicle::updateStateVector(){
	vec_t t(NETWORK_INPUT_NUM);

	if(!USE_CNN){
		std::copy(distances_from_sensors_.begin(), distances_from_sensors_.end(), t.begin());
		int count = distances_from_sensors_.size();

		/*
		* 	State에 추가할 수 있는데 데이터 (속도 / 방향 / 위치 등)
		*	[같이 수정 1]
		*/
		//t[count++] = getSpeed(); 
		//t[count++] = direction_degree_/360.0f;	// 넣을 필요 있는지 모르겠음 (범위: 0~1)
		//t[count++] = center_.x;
		//t[count++] = center_.y;	
	} else {
		makeImage(t);
	}
	past_states_.push_back(t);
	if (past_states_.size() > INPUT_FRAME_CNT) {
		past_states_.pop_front();
	}
}

bool AIVehicle::getState(vec_t& t){
	if (past_states_.size() < INPUT_FRAME_CNT) { t.clear(); return false; }
	for(int i = 0, count = 0; i < INPUT_FRAME_CNT ; ++i, count += NETWORK_INPUT_NUM)
		std::copy(past_states_[i].begin(), past_states_[i].end(), t.begin() + count);	
	return true;
}

// CNN으로 입력 받기 위해 이미지 작업 하다 냅둔 것
void AIVehicle::makeImage(vec_t& t){	

	//std::vector<float> t(WIDTH*HEIGHT, 0.0f);
	const float obstacle = 0.1f;
	const float mycar = 0.5f;

	for (int i = sensor_min, count = 0; i <= sensor_max; i += sensor_di, ++count) {
		const float r = distances_from_sensors_[count]; 
		if(r != 1.0f){
			const float x = (WIDTH/2)*r*sin(glm::radians(float(i))) ;
			const float y = -(HEIGHT/2)*r*cos(glm::radians(float(i))) ;
			//std::cout<< i << "' x:" << (WIDTH/2) + x << " y:" << (HEIGHT/2) + y << endl;
			t[ (WIDTH/2) + (int)x + WIDTH*((HEIGHT/2) + (int)y)] = obstacle;
		}
		//std::cout<< i << "' r:" << r << endl;
	}
	t[(WIDTH/2) + WIDTH*(HEIGHT/2)] = mycar; // this car

	//for(int i = 0; i < HEIGHT; ++i){
	//	for(int j = 0; j < WIDTH; ++j)
	//		std::cout << t[i*WIDTH + j] << " ";
	//	std::cout << endl;
	//}
	//std::cout << endl << endl;
}

bool AIVehicle::validatePos(const float& x, const float& y){
	SelfDrivingWorld& world = SelfDrivingWorld::get();
	glm::vec3 newPos(x, y, 0.0f);

	for (int o = 0; o < world.getVehicles().size(); ++o) {
		if(ID_ & 1 << o)	continue;
		float dist = glm::distance(newPos, world.getVehicles()[o]->center_);
		if(dist < sensing_radius) return false;
	}
	return true;
}

/*
	- 이동체 생성 위치 
	- 지나간 위치에 스키드 마크 제거 (default: off)
	- state 큐 리셋
*/
// reset car status until new position is good enough to do new episode 
void AIVehicle::initialize() {
	const float	world_center_x = 0.5f,
				world_center_y = 0.5,
				world_radius = 1.2f;
	float x,y;
	while (init_sucess_ > 0){
		const float min_x = world_center_x - (world_radius - car_length_);
		const float max_x = world_center_x + (world_radius - car_length_);
		const float min_y = world_center_y - (world_radius - car_length_);
		const float max_y = world_center_y + (world_radius - car_length_);

		x = uniform_rand(min_x, max_x);
		y = uniform_rand(min_y, max_y);

		if( validatePos(x,y) &&
			(std::abs(x - min_x) < 0.1f || std::abs(x - world_center_x) < 0.2f ||std::abs(x - max_x) < 0.1f )&& 
			(std::abs(y - min_x) < 0.2f ||  std::abs(y - world_center_y) < 0.2f || std::abs(y - max_y) < 0.2f)
		){
			initPos(glm::vec3(x, y, 0.0f), car_length_, car_length_);
			previous_pos_ = center_;

			if( x > world_center_x) direction_degree_ = 180.0f;
			else direction_degree_ = 0.0f;
			//direction_degree_ = uniform_rand(0.0f,359.9f);
			setDirection(direction_degree_);

			updateSensor();
			// 시작하자마자 거리가 좁혀져 있을 경우 
			if(isTerminated() == false){
				init_sucess_ = INIT_TRY_CNT;
				break;
			}
		} 
		--init_sucess_;
	}

	past_states_.clear();
}

/*
	이동체 방향 셋팅 
*/
void AIVehicle::setDirection(const float& dir){

	model_matrix_ = glm::mat4();
	dir_ = glm::vec3(01.0f, 0.0f, 0.0f);
	vel_ = glm::vec3(0.002f, 0.0f, 0.0f);

	const glm::mat4 rot_mat = glm::rotate(glm::mat4(), glm::radians(dir), glm::vec3(0, 0, 1));
	glm::vec4 temp(dir_.x, dir_.y, dir_.z, 0.0f);
	temp = rot_mat * temp;
	dir_.x = temp.x;	dir_.y = temp.y;
	rotateCenteredZAxis(dir);
	vel_ = dir_ * glm::sqrt(glm::dot(vel_, vel_));
}

void AIVehicle::turnLeft() {
	const glm::mat4 rot_mat = glm::rotate(glm::mat4(), glm::radians(turn_coeff_), glm::vec3(0, 0, 1));
	glm::vec4 temp(dir_.x, dir_.y, dir_.z, 0.0f);
	temp = rot_mat * temp;
	dir_.x = temp.x;	dir_.y = temp.y;

	rotateCenteredZAxis(turn_coeff_);
	direction_degree_ += turn_coeff_;
	if(direction_degree_ > 360.0f) direction_degree_ -= 360.0f;

	float x = (glm::dot(vel_, dir_) < 0.0)? -1.0: 1.0;
	vel_ = dir_ * glm::sqrt(glm::dot(vel_, vel_)) * x;
}

void AIVehicle::turnRight() {
	const glm::mat4 rot_mat = glm::rotate(glm::mat4(), glm::radians(-turn_coeff_), glm::vec3(0, 0, 1));
	glm::vec4 temp(dir_.x, dir_.y, dir_.z, 0.0f);
	temp = rot_mat * temp;
	dir_.x = temp.x;	dir_.y = temp.y;

	rotateCenteredZAxis(-turn_coeff_);
	direction_degree_ -= turn_coeff_;
	if(direction_degree_ < 0) direction_degree_ = 360.0f - direction_degree_;

	float x = (glm::dot(vel_, dir_) < 0.0)? -1.0: 1.0;
	vel_ = dir_ * glm::sqrt(glm::dot(vel_, vel_)) * x;
}

void AIVehicle::accel(float coeff) {
	vel_ += coeff * dir_;
}

/*
	뒤로 갈 때와 속도를 줄일때 쓰는 계수 틀림~ 
*/
void AIVehicle::decel() {
	if (glm::dot(vel_ - brake_coeff_ * dir_, dir_) > 0.0f )	vel_ -= brake_coeff_ * dir_;	// for fast stop
	else													vel_ -= accel_coeff_ * dir_;	// normally move backward like forward-move
}

/*
	Reward Policy 는 여기에~
	단, episode가 끝났을 때는 updateAll(...)함수에서 -1로 강제 셋팅 
*/
void AIVehicle::calculateReward(float& reward){
	reward = 1.0f;

	float head_sensor_stat = 1.0f;
	float collision_warning = 1.0f;
	for (int i = sensor_min, count = 0; i <= sensor_max; i += sensor_di, ++count) {
		if ( -10 <= i && i <= 10){
			head_sensor_stat = std::min (head_sensor_stat, distances_from_sensors_[count]);
		}	
		collision_warning = std::min (collision_warning, distances_from_sensors_[count]);
	}

	// 속도가 빠를 수록, 정면 센서에 장애물이 없을 수록 보상 증가 
	if(collision_warning <= 0.5f)	reward += collision_warning - 1.0f;
	reward += getSpeed(); 	// keeping moving is reward
	if(reward > 0) reward += head_sensor_stat/2;				// addition reward if head sersors do not detect any obstacle
	reward = floorf(reward * 10) / 10.0f;
}

/*
	DNN에서 받은 action으로 모든 값 업데이트 
*/
void AIVehicle::updateAll(label_t& action, float& reward, vec_t& t) {
	processInput(action);

	vel_ *= (1.0f - fric);	// natural speed reduce 
	model_matrix_ = glm::translate(vel_) * model_matrix_;
	previous_pos_ = center_;
	center_ += vel_;

	updateSensor();
	updateStateVector();

	calculateReward(reward);
	
	if (isTerminated()) {
		reward = -1.0f;		// punishment!!
		initialize();
		t.clear();
	} else {
		getState(t);
	}
}

/*
	주위 오브젝트를 검사하여 센서 길이 없데이트 (0.0 ~ 1.0)
*/
void AIVehicle::updateSensor()
{
	SelfDrivingWorld& world = SelfDrivingWorld::get();
	std::vector<glm::vec3> sensor_lines;
	const glm::vec3 center = center_;
	const float radius = sensing_radius;

	int count = 0;
	for (int i = sensor_min; i <= sensor_max; i += sensor_di, ++count) {
		glm::vec4 end_pt = glm::vec4(radius*cos(glm::radians((float)i)), radius*-sin(glm::radians((float)i)), 0.0f, 0.0f);

		// 방향 별 센서의 길이를 다르게 하는 경우 사용 - 아래는 정면을 길게하는 내용 
		// reduce detecting area of beside sensors by 1/2 
		//end_pt = model_matrix_ * end_pt * (float)(1 - std::abs(i) * 0.80 / sensor_max);
		end_pt = model_matrix_ * end_pt;
		const glm::vec3 r = center + glm::vec3(end_pt.x, end_pt.y, end_pt.z);

		int flag = 0;	
		glm::vec3 col_pt;	
		float min_t = 1e8;

		const std::vector<std::vector<std::unique_ptr<Object>>>& objs = world.getObjs();

		for(int i = 0 ; i < objs.size(); ++i) {
			for(int j = 0; j < objs[i].size(); ++j) {
				if(i == SelfDrivingWorld::OBJ_GR_VEH && ID_ & 1 << j)	continue;

				int flag_temp;	float t_temp;	glm::vec3 col_pt_temp;	
				objs[i][j]->checkCollisionLoop(center, r, flag_temp, t_temp, col_pt_temp);

				if (flag_temp == 1 && t_temp < min_t) {
					min_t = t_temp;	col_pt = col_pt_temp; 
					flag = flag_temp;
				}
			}
		}
		
		if (flag == 1) {
			sensor_lines.push_back(center);
			sensor_lines.push_back(col_pt);

			// scale value between 0.0 ~ 1.0f 
			distances_from_sensors_[count] = glm::distance(col_pt, center) / glm::distance(r, center);
			//distances_from_sensors_[count] = glm::distance(col_pt, center) / sensing_radius;
			
			// for making discrete data
			//distances_from_sensors_[count] = floorf(distances_from_sensors_[count] * 10) / 10.0f;
		}
		else {
			sensor_lines.push_back(center);
			sensor_lines.push_back(r);

			distances_from_sensors_[count] = 1.0f;
			//distances_from_sensors_[count] = glm::distance(r, center) / sensing_radius;
		}
	}
	if(world.is_training() & ID_)	sensing_lines.update(sensor_lines);
	else							sensing_lines.showOff();
}

// 충돌 검사 
bool AIVehicle::isTerminated(){
	bool ret = false;
	glm::vec3 col_line_center;

	const std::vector<std::vector<std::unique_ptr<Object>>>& objs = SelfDrivingWorld::get().getObjs();
	for(int i = 0 ; i < objs.size(); ++i) {
		for(int j = 0; j < objs[i].size(); ++j) {
			if(i == SelfDrivingWorld::OBJ_GR_VEH) {
				if(ID_ & 1 << j)	continue;
				bool crash = checkCollisionLoop(*objs[i][j], col_line_center); // 다른 차와 충돌 체크 
				if(crash) objs[i][j]->initialize();
				ret |= crash;  
			}else {
				ret |= checkCollisionLoop(*objs[i][j], col_line_center); 		// object 충돌 체크 
			}
		}
	}

	ret |= (getSpeed() < -0.50f);	// to prevent go backward

	return ret;
}

// range -1.0f ~ 1.0f
float AIVehicle::getSpeed(){
	float ret = 110.0f * glm::distance(previous_pos_, center_ );
	if (ret < 0.1f) 				ret = 0.0f;		// 빙글 빙글 도는 상태를 막기 위해
	if (glm::dot(vel_, dir_) < 0.0) ret *= -1.0f;	// 방향 factor 
	if (ret < -1.0f) 				ret = -1.0f;	// 최소 -1.0
	if (ret > 1.0f) 				ret = 1.0f;		// 최대 +1.0
	return ret ;
}

/*
	이동체 방향
*/
void AIVehicle::processInput(const int& action){
	switch (action)
	{
	case ACT_ACCEL:
		accel(); 
		break;
	case ACT_LEFT:
		turnLeft();
		accel(0.00005f);
		break;
	case ACT_RIGHT:
		turnRight();
		accel(0.00005f);
		break;
	case ACT_BRAKE:
		decel();
		break;
	default:
		std::cout << "Wrong action - " << action << endl; exit(1); break;
	}
}

/*
	- SelfDrivingWorld Class에서 모든 이동체의 drive호출 하도록 구성
*/
void AIVehicle::update() {
//void AIVehicle::drive(){	

	if(init_sucess_ != INIT_TRY_CNT){
		init_sucess_ = INIT_TRY_CNT;
		initialize();
		return;	
	} 

	std::unique_ptr<Transition> t_ptr = std::make_unique<Transition>(NETWORK_INPUT_NUM * INPUT_FRAME_CNT, 0, 0.0f, NETWORK_INPUT_NUM * INPUT_FRAME_CNT,100.0f);
	vec_t& state 		= std::get<0>(*t_ptr);
	label_t& action 	= std::get<1>(*t_ptr);
	float& reward 		= std::get<2>(*t_ptr);
	vec_t& next_state 	= std::get<3>(*t_ptr);
	float& td 			= std::get<4>(*t_ptr);
	SelfDrivingWorld& world = SelfDrivingWorld::get();

	// get state 
	if(getState(state)) { 
		action = dnn_->getAction(state);
	}

	// get reward and next_statte
	updateAll(action, reward, next_state);

	if(state.size() && world.is_training() &ID_) {
		dnn_->train(state, action, reward, next_state);
	}
}

/*
	이동체 렌더링 함수로 world class에서 호출
*/
void AIVehicle::render(const GLint& MatrixID, const glm::mat4 vp){
	Object::render(MatrixID, vp);
	sensing_lines.render(MatrixID, vp);
}

// end of file 
