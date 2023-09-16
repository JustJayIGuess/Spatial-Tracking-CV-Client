// Usage: <threshold (0.0-1.0)> <horizontal FOV> <vertical FOV>

#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <iostream>
//#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define PORT 1338
#define BROAD_PORT 1339
#define MAXLINE 64
#define REQUEST_MESSAGE "STIPRQST"      // Spatial Tracking IP ReQueST
#define RESPONSE_MESSAGE "STIPRSPN"     // Spatial Tracking IP ReSPoNse
#define VISUAL true

static cv::Point mousePosition(0, 0);
static int clickCount = 0;
static cv::Point clickPos1(0, 0);
static cv::Point clickPos2(0, 0);

static double dist = 0.0;

static float threshold = 0.8f;
static int windowWidth = 0;
static int windowHeight = 0;
static float horizontalFOV = 0.0f;
static float verticalFOV = 0.0f;

static bool liveAdjustThresh = false;

#if VISUAL
void mouse_callback(int  event, int  x, int  y, int  flag, void* param)
{
	if (event == cv::EVENT_MOUSEMOVE && liveAdjustThresh) {
#ifdef VERBOSE_MOUSEMOVE
		cout << "(" << x << ", " << y << ")" << endl;
#endif // VERBOSE_MOUSEMOVE

		mousePosition.x = x;
		mousePosition.y = y;

		threshold = (float)x / windowWidth;
	}
	else if (event == cv::EVENT_LBUTTONDOWN)
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
		dist = cv::norm(clickPos2 - clickPos1);

		std::cout << dist << std::endl;
	}
}
#endif

bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
	double i = std::fabs(cv::contourArea(cv::Mat(contour1)));
	double j = std::fabs(cv::contourArea(cv::Mat(contour2)));
	return (i < j);
}

int main(int argc, char** argv) {
	// Program options
	po::options_description desc("Usage");
	desc.add_options()
		("help", "print help message")
		("horizontal,h", po::value<float>(&horizontalFOV)->default_value(62.2f), "horizontal FOV")
		("vertical,v", po::value<float>(&verticalFOV)->default_value(48.8f), "vertical FOV")
		("threshold,t", po::value<float>(&threshold)->implicit_value(0.9f), "threshold brightness (0.0-1.0)")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}

	// OpenCV Variables
	cv::Mat image;
	cv::Mat splitChannels[3];

#if VISUAL
	cv::namedWindow("Display window");
	cv::namedWindow("Alpha");
	cv::setMouseCallback("Display window", mouse_callback);
#endif

    // if (argc == 4) {
    //     threshold = atof(argv[1]);
        // horizontalFOV = atof(argv[2]);
    //     verticalFOV = atof(argv[3]);
    // }
    // else {
    //     liveAdjustThresh = true;
    // }

	if (horizontalFOV == 0.0f || verticalFOV == 0.0f)
	{
		liveAdjustThresh = true;
	}
	

	cv::VideoCapture cap(0, cv::CAP_V4L2/*, CAP_DSHOW*/);

//	cap.set(CAP_PROP_FRAME_WIDTH, 1920);
//	cap.set(CAP_PROP_FRAME_HEIGHT, 1080);

	if (!cap.isOpened()) {

		std::cout << "cannot open camera";

	}

	cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
	cap.set(cv::CAP_PROP_EXPOSURE, 0);
	cap.set(cv::CAP_PROP_FPS, 30);

#if VISUAL
	cv::Scalar colour(0, 0, 255);
	cv::Scalar colour1(0, 255, 0);
	cv::Scalar colour2(255, 0, 0);
#endif

	cv::Mat alpha;

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cap >> image;
	windowWidth = image.cols;
	windowHeight = image.rows;

	// Server variables
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, broadaddr, anyaddr;

	// Socket fd
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("Socket couldn\'t be created.");
			exit(EXIT_FAILURE);
	}

	int broadcastEnable = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&broadaddr, 0, sizeof(broadaddr));
	memset(&anyaddr, 0, sizeof(anyaddr));

	// Broadcast address - for requesting service IP from server
	broadaddr.sin_family = AF_INET;
	broadaddr.sin_addr.s_addr = INADDR_BROADCAST;
	broadaddr.sin_port = htons(BROAD_PORT);

	// Recieve IP info from server response to broadcast from any interface
	anyaddr.sin_family = AF_INET;
	anyaddr.sin_addr.s_addr = INADDR_ANY;
	anyaddr.sin_port = htons(BROAD_PORT);

	// Store results from recvfrom()'s later.
	socklen_t len;
	int n;

	// Request server IP
	const char* requestmessage = REQUEST_MESSAGE;
	const char* responsemessage = RESPONSE_MESSAGE;
	sendto(sockfd, (const char*)requestmessage, strlen(requestmessage), MSG_CONFIRM, (const struct sockaddr*)&broadaddr, sizeof(broadaddr));
	std::cout << "Sent server connection request, awating response..." << std::endl;

	while (true) {
			len = sizeof(anyaddr);
			n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&anyaddr, &len);
			buffer[n] = '\0';
			if (strcmp(buffer, responsemessage) == 0) {
					printf("\tServer responded with %s, addr=%x\n", buffer, anyaddr.sin_addr.s_addr);

					// Server info
					servaddr.sin_family = AF_INET;
					servaddr.sin_port = htons(PORT);
					servaddr.sin_addr.s_addr = anyaddr.sin_addr.s_addr;
					break;
			}
	}

	while (true) {
		cap >> image;
		cv::cvtColor(image, alpha, cv::COLOR_BGR2GRAY);
		cv::blur(alpha, alpha, cv::Size(3, 3));

		cv::threshold(alpha, alpha, threshold * 255.0, 255, cv::THRESH_BINARY);

		cv::findContours(alpha, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		if (contours.size() > 0)
		{
			std::sort(contours.begin(), contours.end(), compareContourAreas);

			cv::Moments largestContour = cv::moments(contours[contours.size() - 1]);
			cv::Point largestContourCentre(largestContour.m10 / largestContour.m00, largestContour.m01 / largestContour.m00);

			float angleX = 0.5f * horizontalFOV * (2.0f * largestContourCentre.x / windowWidth - 1.0f) + 0.1f;
			float angleY = 0.5f * verticalFOV * (2.0f * largestContourCentre.y / windowHeight - 1.0f) + 0.1f;
			// std::cout << "angle: " << angleX << ", " << angleY << " degrees" << std::endl;


#if VISUAL
			cv::drawContours(image, contours, -1, cv::Scalar(0, 255, 0), 1);
			cv::circle(image, largestContourCentre, 5, colour1, -1);
#endif

			std::string input = std::to_string(angleX) + "," + std::to_string(angleY);

			const char* message = input.c_str();
			std::cout << "Sending: " << message << std::endl;
			sendto(sockfd, (const char*)message, strlen(message), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));

			n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &servaddr, &len);
			buffer[n] = '\0';

			if (strcmp(buffer, RESPONSE_MESSAGE)) {
				std::string stat = "";
				stat += "Data sending.";
				time_t t = time(NULL);
				for (int i = 0; i < t % 3; i++)
				{
					stat += ".";
				}
				std::cout << stat << "  \r";
				std::cout.flush();
			}
		}
		else {
			std::cout << "No data.          \r";
			std::cout.flush();
		}

#if VISUAL
		cv::imshow("Display window", image);
		cv::imshow("Alpha", alpha);
#endif
		cv::waitKey(1);
	}

	close(sockfd);
	return 0;
}
