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
	// �}�E�X�p�̕ϐ�
	struct mouseparam{
		int x;
		int y;
		int event;
		int flags;
	};

	// �}�E�X�p�̃R�[���o�b�N�֐�
	static void callback(int eventType, int x, int y, int flags, void* userdata);


};