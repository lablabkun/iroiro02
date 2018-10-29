#include "Kinect01.h"
#include "opencv01.h"

void main()
{
	try{
		KinectApp app;
		app.initialize_kinect();
		app.initialize_body();


		while (1){
			app.updateBodyFrame();
			app.drawBodyFrame();


			auto key = cv::waitKey(10);
			if (key == 'q'){
				break;
			}

		}
		
	}

	catch (std::exception& ex){
		std::cout << ex.what() << std::endl;
	}

}