#include "Kinect01.h"

KinectApp::~KinectApp()
{
	if (kinect != nullptr){						//もしkinectが終了してなかったら閉じる
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
	//Bodyを取得する
	CComPtr<IBodyFrameSource> bodyFS;
	ERROR_CHECK(kinect->get_BodyFrameSource(&bodyFS));
	ERROR_CHECK(bodyFS->OpenReader(&bodyFR));

	for (auto& body : bodies){					//bodyの配列を空にする
		body = nullptr;
	}
}

void KinectApp::initialize_color()
{
	//COLORについて
	// カラーリーダーの取得
	CComPtr<IColorFrameSource> colorFS;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFS));
	ERROR_CHECK(colorFS->OpenReader(&colorFR));

	// カラー画像のサイズを取得する
	CComPtr<IFrameDescription> defcolorFD;
	ERROR_CHECK(colorFS->get_FrameDescription(&defcolorFD));
	ERROR_CHECK(defcolorFD->get_Width(&colorW));
	ERROR_CHECK(defcolorFD->get_Height(&colorH));
	ERROR_CHECK(defcolorFD->get_BytesPerPixel(&colorBPP));
	std::cout << "default : " << colorW << "," << colorH << "," << colorBPP << std::endl;

	// カラー画像のサイズを取得する
	CComPtr<IFrameDescription> colorFD;
	ERROR_CHECK(colorFS->CreateFrameDescription(colorFormat, &colorFD));
	ERROR_CHECK(colorFD->get_Width(&colorW));
	ERROR_CHECK(colorFD->get_Height(&colorH));
	ERROR_CHECK(colorFD->get_BytesPerPixel(&colorBPP));
	std::cout << "create : " << colorW << "," << colorH << "," << colorBPP << std::endl;

	// バッファを作成する
	colorBuff.resize(colorW * colorH * colorBPP);
}

void KinectApp::updateBodyFrame()
{
	//フレームを取得する
	CComPtr<IBodyFrame> bodyF;
	auto ret = bodyFR->AcquireLatestFrame(&bodyF);
	if (FAILED(ret)){							//もし最新のフレームを受け取ることができなかったらもっかいする
		return;
	}

	//前回のbodyを解放する
	for (auto& body : bodies){
		if (body != nullptr){
			body->Release();
			body = nullptr;
		}
	}

	//データを取得する
	ERROR_CHECK(bodyF->GetAndRefreshBodyData(6, &bodies[0]));
}

void KinectApp::updateColorFrame()
{
	//フレームを取得する
	CComPtr<IColorFrame> colorF;
	auto ret = colorFR->AcquireLatestFrame(&colorF);
	if (FAILED(ret)){					//フレームを取得できなかったらもどる
		return;
	}

	//指定の形式でデータを取得する
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

		// 関節の位置を表示する
		Joint joints[JointType::JointType_Count];
		body->GetJoints(JointType::JointType_Count, joints);
		for (auto joint : joints) {
			// 手の位置が追跡状態
			if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
				drawEllipse(bodyImage, joint, 10, cv::Scalar(255, 0, 0));
			}
			// 手の位置が推測状態
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
	// カメラ座標系をDepth座標系に変換する
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

	DepthSpacePoint point;
	mapper->MapCameraPointToDepthSpace(joint.Position, &point);

	cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
}


// *****お絵かき*********************************************
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

		joint = joints[num];			//ジョイントの位置の受け渡し
		body->get_HandLeftState(&hand);		//左手の状態の受け渡し

		return;
	}	
}

void KinectApp::bodynum_joint(int num, both_hands &hands)
{
	for (int i = 0; i < 6; i++){
		if (bodies[i] == nullptr){	// bodyになにもなかったら続けるだけ
			continue;
		}
		else{						// bodyになにかあった場合はいろいろする
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