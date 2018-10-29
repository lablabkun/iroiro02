// include ++++++++++++++++++++
//---kinect
#include <Kinect.h>
//---opencv
#include <opencv2\opencv.hpp>
//---another
#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <map>

// ERROR CHECK
//ERROR_CHECK( ::GetDefaultKinectSeneor(&kinect));
#define ERROR_CHECK(ret) if((ret) != S_OK){std::stringstream ss; ss << "failed" #ret " " << std::hex << ret<< std::endl; throw std::runtime_error(ss.str().c_str());}

// Kinect Class ++++++++++++++++
class KinectApp
{
private:
	//---Kinect�S��
	CComPtr<IKinectSensor> kinect = nullptr;		//kinect�̐ݒ�
	//---Body�p
	CComPtr<IBodyFrameReader> bodyFR = nullptr;		//bodyFrameReader�̐ݒ�
	IBody* bodies[6];								//�Ƃ��body�̐���6�܂�
	//---Color�p
	CComPtr<IColorFrameReader> colorFR = nullptr;
	int colorW;		//�J���[�摜�̕�
	int colorH;		//�J���[�摜�̍���
	unsigned int colorBPP;	//�΂��Ƃρ[�҂�����
	ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;	//�J���[�t�H�[�}�b�g
	std::vector<BYTE> colorBuff;

public:
	// ---����---------------------------
	// Kinect�̏I������
	~KinectApp();
	// Kinect�̏�����
	void initialize_kinect();
	// �{�f�B�̏�����
	void initialize_body();
	// �J���[�摜�̏�����
	void initialize_color();
	// �{�f�B�̍X�V
	void updateBodyFrame();
	// �J���[�摜�̎擾
	void updateColorFrame();
	// �{�f�B�̕`��
	void drawBodyFrame();
	// �J���[�摜�̕`��
	void drawColorFrame();
	// �֐߂̕`��
	void drawEllipse(cv::Mat& bodyImage, const Joint& joint, int r, const cv::Scalar& color);

	// ---���G�����p--------------------------
	// �J���[�摜(����)�̕`�恕�摜���󂯓n��
	cv::Mat drawColorFrame_oekaki();
	// �I�������W���C���g�̈ʒu��Ԃ�
	Joint body_joint(int num);
	// �I�������W���C���g�̈ʒu��Ԃ�������̏�Ԃ�Ԃ�
	void body_joint2(int num, Joint &joint, HandState &hand);
	// �{�f�B���ƂɃW���C���g�̈ʒu�ƍ���̏�Ԃ�Ԃ�
	//---�ϐ��̒�`
	struct both_hands{
		Joint joint;
		HandState hidari;
	};
	//---�֐��̒�`
	void bodynum_joint(int num, both_hands &hands);

};