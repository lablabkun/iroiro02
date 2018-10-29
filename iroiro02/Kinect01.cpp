#include "Kinect01.h"

KinectApp::~KinectApp()
{
	if (kinect != nullptr){						//����kinect���I�����ĂȂ����������
		kinect->Close();
	}
}

void KinectApp::initialize_kinect()
{
	ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
	ERROR_CHECK(kinect->Open());
}

void KinectApp::initialize_body()
{
	//Body���擾����
	CComPtr<IBodyFrameSource> bodyFS;
	ERROR_CHECK(kinect->get_BodyFrameSource(&bodyFS));
	ERROR_CHECK(bodyFS->OpenReader(&bodyFR));

	for (auto& body : bodies){					//body�̔z�����ɂ���
		body = nullptr;
	}
}

void KinectApp::initialize_color()
{
	//COLOR�ɂ���
	// �J���[���[�_�[�̎擾
	CComPtr<IColorFrameSource> colorFS;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFS));
	ERROR_CHECK(colorFS->OpenReader(&colorFR));

	// �J���[�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription> defcolorFD;
	ERROR_CHECK(colorFS->get_FrameDescription(&defcolorFD));
	ERROR_CHECK(defcolorFD->get_Width(&colorW));
	ERROR_CHECK(defcolorFD->get_Height(&colorH));
	ERROR_CHECK(defcolorFD->get_BytesPerPixel(&colorBPP));
	std::cout << "default : " << colorW << "," << colorH << "," << colorBPP << std::endl;

	// �J���[�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription> colorFD;
	ERROR_CHECK(colorFS->CreateFrameDescription(colorFormat, &colorFD));
	ERROR_CHECK(colorFD->get_Width(&colorW));
	ERROR_CHECK(colorFD->get_Height(&colorH));
	ERROR_CHECK(colorFD->get_BytesPerPixel(&colorBPP));
	std::cout << "create : " << colorW << "," << colorH << "," << colorBPP << std::endl;

	// �o�b�t�@���쐬����
	colorBuff.resize(colorW * colorH * colorBPP);
}

void KinectApp::updateBodyFrame()
{
	//�t���[�����擾����
	CComPtr<IBodyFrame> bodyF;
	auto ret = bodyFR->AcquireLatestFrame(&bodyF);
	if (FAILED(ret)){							//�����ŐV�̃t���[�����󂯎�邱�Ƃ��ł��Ȃ������������������
		return;
	}

	//�O���body���������
	for (auto& body : bodies){
		if (body != nullptr){
			body->Release();
			body = nullptr;
		}
	}

	//�f�[�^���擾����
	ERROR_CHECK(bodyF->GetAndRefreshBodyData(6, &bodies[0]));
}

void KinectApp::updateColorFrame()
{
	//�t���[�����擾����
	CComPtr<IColorFrame> colorF;
	auto ret = colorFR->AcquireLatestFrame(&colorF);
	if (FAILED(ret)){					//�t���[�����擾�ł��Ȃ���������ǂ�
		return;
	}

	//�w��̌`���Ńf�[�^���擾����
	ERROR_CHECK(colorF->CopyConvertedFrameDataToArray(colorBuff.size(), &colorBuff[0], colorFormat));
}

void KinectApp::drawBodyFrame()
{
	cv::Mat bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);

	for (auto body : bodies){
		if (body == nullptr){
			continue;
		}

		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (!isTracked) {
			continue;
		}

		// �֐߂̈ʒu��\������
		Joint joints[JointType::JointType_Count];
		body->GetJoints(JointType::JointType_Count, joints);
		for (auto joint : joints) {
			// ��̈ʒu���ǐՏ��
			if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
				drawEllipse(bodyImage, joint, 10, cv::Scalar(255, 0, 0));
			}
			// ��̈ʒu���������
			else if (joint.TrackingState == TrackingState::TrackingState_Inferred) {
				drawEllipse(bodyImage, joint, 10, cv::Scalar(255, 255, 0));
			}
		}
	}

	cv::imshow("Body Image", bodyImage);
}

void KinectApp::drawColorFrame()
{

}

void KinectApp::drawEllipse(cv::Mat& bodyImage, const Joint& joint, int r, const cv::Scalar& color)
{
	// �J�������W�n��Depth���W�n�ɕϊ�����
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

	DepthSpacePoint point;
	mapper->MapCameraPointToDepthSpace(joint.Position, &point);

	cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
}


// *****���G����*********************************************
cv::Mat KinectApp::drawColorFrame_oekaki()
{
	cv::Mat colorImage(colorH, colorW, CV_8UC4, &colorBuff[0]);
	cv::Mat halfImage;
	cv::resize(colorImage, halfImage, cv::Size(), 0.5, 0.5);
	return halfImage;
}


Joint KinectApp::body_joint(int num)
{
	for (auto body : bodies){
		if (body == nullptr){
			continue;
		}

		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (!isTracked) {
			continue;
		}

		Joint joints[JointType::JointType_Count];
		body->GetJoints(JointType::JointType_Count, joints);

		return joints[num];
	}
}

void KinectApp::body_joint2(int num, Joint &joint, HandState &hand)
{
	for (auto body : bodies){
		if (body == nullptr){
			continue;
		}

		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (!isTracked) {
			continue;
		}

		Joint joints[JointType::JointType_Count];
		body->GetJoints(JointType::JointType_Count, joints);

		joint = joints[num];			//�W���C���g�̈ʒu�̎󂯓n��
		body->get_HandLeftState(&hand);		//����̏�Ԃ̎󂯓n��

		return;
	}	
}

void KinectApp::bodynum_joint(int num, both_hands &hands)
{
	for (int i = 0; i < 6; i++){
		if (bodies[i] == nullptr){	// body�ɂȂɂ��Ȃ������瑱���邾��
			continue;
		}
		else{						// body�ɂȂɂ��������ꍇ�͂��낢�낷��
			BOOLEAN isTracked = false;
			ERROR_CHECK(bodies[i]->get_IsTracked(&isTracked));
			if (!isTracked) {
				continue;
			}

			Joint joints[JointType::JointType_Count];
			bodies[i]->GetJoints(JointType::JointType_Count, joints);

			hands[i].joint = joints[num];
			bodies[i]->get_HandLeftState(&hands[i].hidari);

		}
	}
}