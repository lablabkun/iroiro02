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
	//---Kinect全般
	CComPtr<IKinectSensor> kinect = nullptr;		//kinectの設定
	//---Body用
	CComPtr<IBodyFrameReader> bodyFR = nullptr;		//bodyFrameReaderの設定
	IBody* bodies[6];								//とれるbodyの数は6個まで
	//---Color用
	CComPtr<IColorFrameReader> colorFR = nullptr;
	int colorW;		//カラー画像の幅
	int colorH;		//カラー画像の高さ
	unsigned int colorBPP;	//ばいとぱーぴくせる
	ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;	//カラーフォーマット
	std::vector<BYTE> colorBuff;

public:
	// ---既存---------------------------
	// Kinectの終了処理
	~KinectApp();
	// Kinectの初期化
	void initialize_kinect();
	// ボディの初期化
	void initialize_body();
	// カラー画像の初期化
	void initialize_color();
	// ボディの更新
	void updateBodyFrame();
	// カラー画像の取得
	void updateColorFrame();
	// ボディの描画
	void drawBodyFrame();
	// カラー画像の描画
	void drawColorFrame();
	// 関節の描画
	void drawEllipse(cv::Mat& bodyImage, const Joint& joint, int r, const cv::Scalar& color);

	// ---お絵かき用--------------------------
	// カラー画像(半分)の描画＆画像を受け渡す
	cv::Mat drawColorFrame_oekaki();
	// 選択したジョイントの位置を返す
	Joint body_joint(int num);
	// 選択したジョイントの位置を返す＆左手の状態を返す
	void body_joint2(int num, Joint &joint, HandState &hand);
	// ボディごとにジョイントの位置と左手の状態を返す
	//---変数の定義
	struct both_hands{
		Joint joint;
		HandState hidari;
	};
	//---関数の定義
	void bodynum_joint(int num, both_hands &hands);

};