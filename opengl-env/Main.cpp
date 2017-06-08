/*
    Copied and Pasted by Jay (JongYoon) Kim, plenum@naver.com 
*/
#include "SelfDrivingWorld.h"
//#include "ctime"

// Maximum ipc connection is 22 !!!
//
// if you want set this to more than 5, 
// then you need to change initial position of vehicles in AICar::initialize()
// it may go inifite loop because of collision with other vehicles 
const int CAR_PLAYED_NUM = 9;	
const int CAR_PLAYED_NUM_MAX = 22;	


int main(int argc, char** argv) {
	//set_random_seed(1);
	
	boost::asio::io_service ios;
	//TestDrivingScene scene;
	SelfDrivingWorld& world = SelfDrivingWorld::get();

	int car_num = CAR_PLAYED_NUM;
	if(argc == 2){
		int arg = atoi(argv[1]);
		car_num = std::min(CAR_PLAYED_NUM_MAX, arg);
	}
	world.initialize(ios, car_num);
	world.run();
	
	return 0;
}


// end of file 
