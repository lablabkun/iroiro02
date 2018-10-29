// include
//---opencv
#include <opencv2\opencv.hpp>

//---another
#include <sstream>
#include <iostream>


// Mouse class
class Mouse
{
public:
	// マウス用の変数
	struct mouseparam{
		int x;
		int y;
		int event;
		int flags;
	};

	// マウス用のコールバック関数
	static void callback(int eventType, int x, int y, int flags, void* userdata);


};