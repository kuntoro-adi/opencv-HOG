#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ml\ml.hpp>

#include <iostream>
#include <fstream>

#include "HOG.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	float dummyMag[8] = {1,1,1,1,1,1,1,1};

	float dummyAng[8] = {0,18,90,99,351,270,144,180};

	Mat mag = Mat(1, 8, CV_32F, dummyMag);
	Mat ang = Mat(1, 8, CV_32F, dummyAng);

	Mat wHogFeature;
	Mat uHogFeature;
	computeHOG(mag, ang, wHogFeature, 8, true);
	computeHOG(mag, ang, uHogFeature, 8, false);

	cout << endl << endl;
	cout << "Magnitude: " << mag << endl;
	cout << "Angle: " << ang << endl;
	cout << "Weighted HOG feature: " << wHogFeature << endl;
	cout << "Unweighted HOG feature: " << uHogFeature << endl;

	std::cout << "done!" << endl;

	std::cin.get();

	return 0;
}