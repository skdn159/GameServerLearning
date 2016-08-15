#include <iostream>
#include <thread>
#include "../../ServerLogic/Main.h"

int main()
{

	ServerLogic::Main serverMain;
	serverMain.Init();

 	std::thread logicThread([&]() {
 
		serverMain.Run(); }
 	);

 	std::cout << "press any key to exit...";
 	getchar();
 
	serverMain.Stop();
 	logicThread.join();

	return 0;
}