#include <opencv2/opencv.hpp>

#define VISUAL true

using namespace cv;

using namespace std;

static Point mousePosition(0, 0);
static int clickCount = 0;
static Point clickPos1(0, 0);
static Point clickPos2(0, 0);

static double dist = 0.0;

static float a1 = 0.8f;
static float a2 = 1.0f;
static float a3 = 0.2f;
static int windowWidth = 0;
static int windowHeight = 0;
static float horizontalFOV;
static float verticalFOV;

static bool liveAdjustA1 = false;

#if VISUAL
void mouse_callback(int  event, int  x, int  y, int  flag, void* param)
{
	if (event == EVENT_MOUSEMOVE && liveAdjustA1) {
#ifdef VERBOSE_MOUSEMOVE
		cout << "(" << x << ", " << y << ")" << endl;
#endif // VERBOSE_MOUSEMOVE

		mousePosition.x = x;
		mousePosition.y = y;

		a1 = (float)x / windowWidth;
		a2 = (float)y / windowHeight;
		a3 = 0.2f * a1;
	}
	else if (event == EVENT_LBUTTONDOWN)
	{
		if (clickCount % 2 == 0)
		{
			clickPos1.x = x;
			clickPos1.y = y;
		}
		else
		{
			clickPos2.x = x;
			clickPos2.y = y;
		}

		clickCount++;
		dist = norm(clickPos2 - clickPos1);

		cout << dist << endl;
	}
}
#endif

bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
	double i = fabs(contourArea(cv::Mat(contour1)));
	double j = fabs(contourArea(cv::Mat(contour2)));
	return (i < j);
}

int main(int argc, char** argv) {
	Mat image;
	Mat splitChannels[3];

#if VISUAL
	namedWindow("Display window");
	namedWindow("Alpha");
	setMouseCallback("Display window", mouse_callback);
#endif

    if (argc == 4) {
        a1 = atof(argv[1]);
        horizontalFOV = atof(argv[2]);
        verticalFOV = atof(argv[3]);
    }
    else {
        liveAdjustA1 = true;
    }



	VideoCapture cap(0/*, CAP_DSHOW*/);

//	cap.set(CAP_PROP_FRAME_WIDTH, 1920);
//	cap.set(CAP_PROP_FRAME_HEIGHT, 1080);

	if (!cap.isOpened()) {

		cout << "cannot open camera";

	}

	cap.set(CAP_PROP_AUTO_EXPOSURE, 1);
    cap.set(CAP_PROP_EXPOSURE, 0);
    cap.set(CAP_PROP_FPS, 30);

#if VISUAL
	Scalar colour(0, 0, 255);
	Scalar colour1(0, 255, 0);
	Scalar colour2(255, 0, 0);
#endif

	Mat alpha;

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	cap >> image;
	windowWidth = image.cols;
	windowHeight = image.rows;

	while (true) {
		cap >> image;
		cvtColor(image, alpha, COLOR_BGR2GRAY);
		blur(alpha, alpha, Size(3, 3));

		threshold(alpha, alpha, a1 * 255.0, 255, THRESH_BINARY);

		findContours(alpha, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

		if (contours.size() > 0)
		{
			sort(contours.begin(), contours.end(), compareContourAreas);

			Moments largestContour = moments(contours[contours.size() - 1]);
			Point largestContourCentre(largestContour.m10 / largestContour.m00, largestContour.m01 / largestContour.m00);

			float angleX = 0.5f * horizontalFOV * (2.0f * largestContourCentre.x / windowWidth - 1.0f);
			float angleY = 0.5f * verticalFOV * (2.0f * largestContourCentre.y / windowHeight - 1.0f);
			cout << "angle: " << angleX << ", " << angleY << " degrees" << endl;


#if VISUAL
			drawContours(image, contours, -1, Scalar(0, 255, 0), 1);
			circle(image, largestContourCentre, 5, colour1, -1);
#endif		
		}

#if VISUAL
		imshow("Display window", image);
		imshow("Alpha", alpha);
#endif

		waitKey(1);
	}

	return 0;

}
